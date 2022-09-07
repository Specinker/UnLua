// Tencent is pleased to support the open source community by making UnLua available.
// 
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and limitations under the License.

#include "Engine/World.h"
#include "LuaEnvLocator.h"
#include "Registries/ClassRegistry.h"
#include "Registries/EnumRegistry.h"
#include "UnLuaInterface.h"
#include "LuaDynamicBinding.h"
#include "UnLuaDelegates.h"


UnLua::FLuaEnv* ULuaEnvLocator::Locate(const UObject* Object)
{
    if (!Env)
    {
        Env = MakeShared<UnLua::FLuaEnv, ESPMode::ThreadSafe>();
        Env->Start();
    }
    return Env.Get();
}

void ULuaEnvLocator::HotReload()
{
    if (!Env)
        return;
    Env->HotReload();
}

void ULuaEnvLocator::Reset()
{
    Env.Reset();
}

UnLua::FLuaEnv* ULuaEnvLocator_ByGameInstance::Locate(const UObject* Object)
{
    if (!Object)
        return GetDefault();

    UGameInstance* GameInstance;
    if (Object->IsA(UGameInstance::StaticClass()))
    {
        GameInstance = (UGameInstance*)Object;
    }
    else
    {
        const auto Outer = Object->GetOuter();
        if (!Outer)
            return GetDefault();

        const auto World = Outer->GetWorld();
        if (!World)
            return GetDefault();

        GameInstance = World->GetGameInstance();
        if (!GameInstance)
            return GetDefault();
    }

    const auto Exists = Envs.Find(GameInstance);
    if (Exists)
        return (*Exists).Get();

    const TSharedPtr<UnLua::FLuaEnv, ESPMode::ThreadSafe> Ret = MakeShared<UnLua::FLuaEnv, ESPMode::ThreadSafe>();
    Ret->SetName(FString::Printf(TEXT("Env_%d"), Envs.Num() + 1));
    Ret->Start();
    Envs.Add(GameInstance, Ret);
    return Ret.Get();
}

void ULuaEnvLocator_ByGameInstance::HotReload()
{
    if (Env)
        Env->HotReload();
    for (const auto& Pair : Envs)
        Pair.Value->HotReload();
}

void ULuaEnvLocator_ByGameInstance::Reset()
{
    Env.Reset();
    for (auto Pair : Envs)
        Pair.Value.Reset();
    Envs.Empty();
}

UnLua::FLuaEnv* ULuaEnvLocator_ByGameInstance::GetDefault()
{
    if (!Env)
    {
        Env = MakeShared<UnLua::FLuaEnv, ESPMode::ThreadSafe>();
        Env->Start();
    }
    return Env.Get();
}

#if WITH_EDITOR     //[modify] by italink
ULuaEnvLocator_Editor::ULuaEnvLocator_Editor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    GUObjectArray.AddUObjectDeleteListener(this);
}

ULuaEnvLocator_Editor* ULuaEnvLocator_Editor::Instance()
{
    static ULuaEnvLocator_Editor* Ins = nullptr;
    if (!Ins) {
        Ins = NewObject<ULuaEnvLocator_Editor>();
        Ins->AddToRoot();
    }
    return Ins;
}

void ULuaEnvLocator_Editor::SetActive(bool bActive)
{
    if (bActive) {
        GUObjectArray.AddUObjectCreateListener(this);
        FUnLuaDelegates::OnObjectBinded.AddUObject(this, &ULuaEnvLocator_Editor::NotifyUObjectBinded);
        if (!Env.IsValid()) {
            Env = MakeShared<UnLua::FLuaEnv>();
            Env->Start();
        }
        else {
            RecoverLuaEnvState();
        }
        for (const auto Object : BoundObjects2ModuleName) {
            FScopedLuaDynamicBinding Binding(Env->GetMainState(), Object.Key->GetClass(), *Object.Value, LUA_NOREF);
            Env->TryBind((UObject*)Object.Key);
        }
    }
    else {
        GUObjectArray.RemoveUObjectCreateListener(this);
        FUnLuaDelegates::OnObjectBinded.RemoveAll(this);
        TakeoutLuaEnvState();
        GPropertyCreator.Cleanup();
        for (const auto Class : TObjectRange<UClass>())
        {
            if (Class->ImplementsInterface(UUnLuaInterface::StaticClass()))
                ULuaFunction::RestoreOverrides(Class);
        }
        for (const auto Object : BoundObjects2ModuleName) {
            Env->GetObjectRegistry()->Unbind((UObject*)Object.Key);
            ULuaFunction::RestoreOverrides(Object.Key->GetClass());
        }
    }
    bIsActive = bActive;
}

void ULuaEnvLocator_Editor::RequestRebind()
{
    auto CurrentBoundObjects2ModuleName = BoundObjects2ModuleName;
    Env->GetManager()->Cleanup();
    Env->GetManager()->Env = Env.Get();
    for (const auto Object : CurrentBoundObjects2ModuleName) {
        Env->GetObjectRegistry()->Unbind((UObject*)Object.Key);
        UnLua::FClassRegistry::StaticUnregister(Object.Key);
        UnLua::FEnumRegistry::StaticUnregister(Object.Key);
        FScopedLuaDynamicBinding Binding(Env->GetMainState(), Object.Key->GetClass(), *Object.Value, LUA_NOREF);
        Env->TryBind((UObject*)Object.Key);
    }
}

void ULuaEnvLocator_Editor::AddBoundObject(UObject* Obj, FString ModuleName)
{
    BoundObjects2ModuleName.FindOrAdd(Obj, ModuleName);
}

void ULuaEnvLocator_Editor::TakeoutLuaEnvState()
{
    LastState.Classes = UnLua::FClassRegistry::Classes;
    LastState.Name2Classes = UnLua::FClassRegistry::Name2Classes;
    LastState.Enums = UnLua::FEnumRegistry::Enums;
    LastState.Name2Enums = UnLua::FEnumRegistry::Name2Enums;

    UnLua::FClassRegistry::Classes.Empty();
    UnLua::FClassRegistry::Name2Classes.Empty();
    UnLua::FEnumRegistry::Enums.Empty();
    UnLua::FEnumRegistry::Name2Enums.Empty();
}

void ULuaEnvLocator_Editor::RecoverLuaEnvState()
{
    UnLua::FClassRegistry::Classes = LastState.Classes;
    UnLua::FClassRegistry::Name2Classes = LastState.Name2Classes;
    UnLua::FEnumRegistry::Enums = LastState.Enums;
    UnLua::FEnumRegistry::Name2Enums = LastState.Name2Enums;
}

void ULuaEnvLocator_Editor::NotifyUObjectBinded(UObjectBaseUtility* Object)
{
    if (!GLuaDynamicBinding.ModuleName.IsEmpty()) {
        AddBoundObject((UObject*)Object, GLuaDynamicBinding.ModuleName);
    }
    else {
        UE_LOG(LogUnLua, Warning, TEXT("%s: Editor object only support dynamic binding "), ANSI_TO_TCHAR(__FUNCTION__));
    }
}

void ULuaEnvLocator_Editor::NotifyUObjectCreated(const UObjectBase* ObjectBase, int32 Index)
{
    if (!bIsActive)
        return;
    UObject* Object = (UObject*)ObjectBase;
    const auto Class = Object->IsA<UClass>() ? static_cast<UClass*>(Object) : Object->GetClass();
    if (GLuaDynamicBinding.IsValid(Class)) {
        Env->TryBind(Object);
        Env->TryReplaceInputs(Object);
    }
}

void ULuaEnvLocator_Editor::NotifyUObjectDeleted(const UObjectBase* Object, int32 Index)
{
    BoundObjects2ModuleName.Remove(Object);
    if (!bIsActive)
        return;
    if (UnLua::FClassRegistry::StaticUnregister(Object))
        return;
    UnLua::FEnumRegistry::StaticUnregister(Object);
}

void ULuaEnvLocator_Editor::OnUObjectArrayShutdown()
{
    if (!bIsActive)
        return;
    GUObjectArray.RemoveUObjectDeleteListener(this);
    GUObjectArray.RemoveUObjectDeleteListener(this);
    bIsActive = false;
}
#endif
