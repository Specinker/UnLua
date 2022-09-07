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

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LuaEnv.h"
#include "LuaEnvLocator.generated.h"

UCLASS()
class UNLUA_API ULuaEnvLocator : public UObject
{
    GENERATED_BODY()
public:
    virtual UnLua::FLuaEnv* Locate(const UObject* Object);

    virtual void HotReload();

    virtual void Reset();

    TSharedPtr<UnLua::FLuaEnv, ESPMode::ThreadSafe> Env;
};

UCLASS()
class UNLUA_API ULuaEnvLocator_ByGameInstance : public ULuaEnvLocator
{
    GENERATED_BODY()
public:
    virtual UnLua::FLuaEnv* Locate(const UObject* Object) override;

    virtual void HotReload() override;

    virtual void Reset() override;

    UnLua::FLuaEnv* GetDefault();

    TMap<TWeakObjectPtr<UGameInstance>, TSharedPtr<UnLua::FLuaEnv, ESPMode::ThreadSafe>> Envs;
};

#if WITH_EDITOR     //[modify] by italink
UCLASS()
class UNLUA_API ULuaEnvLocator_Editor : public ULuaEnvLocator, public FUObjectArray::FUObjectDeleteListener, public FUObjectArray::FUObjectCreateListener
{
    GENERATED_UCLASS_BODY()
public:
    static ULuaEnvLocator_Editor* Instance();
    void SetActive(bool bActive);
    void RequestRebind();
    void AddBoundObject(UObject* Obj, FString ModuleName);

    void TakeoutLuaEnvState();
    void RecoverLuaEnvState();

protected:
    virtual void NotifyUObjectBinded(UObjectBaseUtility* Object);
    virtual void NotifyUObjectCreated(const UObjectBase* ObjectBase, int32 Index) override;
    virtual void NotifyUObjectDeleted(const UObjectBase* Object, int32 Index) override;
    virtual void OnUObjectArrayShutdown() override;

    TMap<const UObjectBase*, FString> BoundObjects2ModuleName;
    bool bIsActive = false;

    struct LuaEnvState {
        TMap<UStruct*, FClassDesc*> Classes;
        TMap<FName, FClassDesc*> Name2Classes;

        TMap<UEnum*, FEnumDesc*> Enums;
        TMap<FName, FEnumDesc*> Name2Enums;

    }LastState;
};
#endif