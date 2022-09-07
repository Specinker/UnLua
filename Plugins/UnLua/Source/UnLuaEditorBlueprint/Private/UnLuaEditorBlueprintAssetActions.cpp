// Fill out your copyright notice in the Description page of Project Settings.

#include "UnLuaEditorBlueprintAssetActions.h"
#include "UnLuaEditorBlueprint.h"
#include "IBlutilityModule.h"
#include "BlueprintEditorModule.h"
#include "ToolMenuSection.h"
#include "IAssetTools.h"
#include "EditorReimportHandler.h"
#include "Widgets/SWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "LevelViewportTabContent.h"
#include "AssetEditorViewportLayout.h"
#include "SLevelViewport.h"
#include "LevelEditor.h"
#include "LevelEditor\Private\SLevelEditor.h"

#define LOCTEXT_NAMESPACE "UnLuaEditorBlueprint"

FText FUnLuaEditorBlueprintAssetActions::GetName() const {
	return LOCTEXT("UnLuaEditorBlueprintAssetActions", "UnLua Edtior Blueprint");
}

UClass* FUnLuaEditorBlueprintAssetActions::GetSupportedClass() const {
	return UUnLuaEditorBlueprint::StaticClass();
}

FColor FUnLuaEditorBlueprintAssetActions::GetTypeColor() const {
	return FColor(0, 100, 200);
}

uint32 FUnLuaEditorBlueprintAssetActions::GetCategories() {
	IBlutilityModule* BlutilityModule = FModuleManager::GetModulePtr<IBlutilityModule>("Blutility");
	return BlutilityModule->GetAssetCategory();
}

bool FUnLuaEditorBlueprintAssetActions::HasActions(const TArray<UObject*>& InObjects) const {
	return true;
}

void FUnLuaEditorBlueprintAssetActions::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	if (InObjects.Num() == 1) {
		UObject* Object = InObjects[0];
		if (GetStartupInstance(Object) == nullptr) {
			Section.AddMenuEntry(
				"FUnLuaEditorBlueprintAssetActions_Edit",
				LOCTEXT("FUnLuaEditorBlueprintAssetActions", "Startup"),
				LOCTEXT("FUnLuaEditorBlueprintAssetActions_Tooltip", "Startup"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([this, InObjects]() {
						for (auto Object : InObjects) {
							TryStartup(Object);
						}
						}),
					FCanExecuteAction()
							)
			);
		}
		else {
			Section.AddMenuEntry(
				"FUnLuaEditorBlueprintAssetActions_Shutdown",
				LOCTEXT("FUnLuaEditorBlueprintAssetActions", "Shutdown"),
				LOCTEXT("FUnLuaEditorBlueprintAssetActions_Tooltip", "Shutdown."),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([this, InObjects]() {
						for (auto Object : InObjects) {
							TryShutdown(Object);
						}
						}),
					FCanExecuteAction()
							)
			);
		}
	}
}

UObject* FUnLuaEditorBlueprintAssetActions::GetStartupInstance(UObject* InObject)
{
	UObject* Instance = MapBpToInstacne.FindRef(InObject);
	if (Instance) {
		TArray<UObject*> Children;
		GetObjectsWithOuter(InObject, Children);
		if (Children.Contains(Instance)) {
			return Instance;
		}
		MapBpToInstacne.Remove(Instance);
	}
	return nullptr;
}

void FUnLuaEditorBlueprintAssetActions::TryStartup(UObject* InObject)
{
	if (!InObject || !IsValidChecked(InObject) || InObject->IsUnreachable())
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not run: %s"), InObject ? *InObject->GetPathName() : TEXT("None"));
	}

	if (UUnLuaEditorBlueprint* Blueprint = Cast<UUnLuaEditorBlueprint>(InObject))
	{
		UClass* ObjectClass = Blueprint->GeneratedClass;
		UFunction* RunFunction = ObjectClass->FindFunctionByName("TryStartup");
		if (RunFunction)
		{
			UObject* Instance = GetStartupInstance(InObject);
			if (Instance == nullptr) {
				Instance = NewObject<UObject>(InObject, ObjectClass);
				MapBpToInstacne.Add(Blueprint, Instance);
				if (GEditor&& !GEditor->OnBlueprintPreCompile().IsBoundToObject(this)) {
					GEditor->OnBlueprintPreCompile().AddSP(this,&FUnLuaEditorBlueprintAssetActions::OnHandleBlueprintPreCompile);
				}
				FEditorScriptExecutionGuard ScriptGuard;
				Instance->ProcessEvent(RunFunction, nullptr);
			}
		}
	}
}

void FUnLuaEditorBlueprintAssetActions::TryShutdown(UObject* InObject)
{
	if (UUnLuaEditorBlueprint* Blueprint = Cast<UUnLuaEditorBlueprint>(InObject))
	{
		UClass* ObjectClass = Blueprint->GeneratedClass;
		UFunction* RunFunction = ObjectClass->FindFunctionByName("TryShutdown");
		if (RunFunction)
		{
			UObject* Instance = GetStartupInstance(InObject);
			if (Instance) {
				if (!Instance->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed)) {
					FEditorScriptExecutionGuard ScriptGuard;
					Instance->ProcessEvent(RunFunction, nullptr);
				}
				MapBpToInstacne.Remove(Blueprint);
			}
		}
	}
}

void FUnLuaEditorBlueprintAssetActions::OnHandleBlueprintPreCompile(UBlueprint* InBlueprint)
{
	if (Cast<UUnLuaEditorBlueprint>(InBlueprint)) {
		TryShutdown(InBlueprint);
	}
	TArray<UObject*> Children;
	GetObjectsWithOuter(InBlueprint, Children);
	for (auto Child : Children) {
		if (Child->GetClass()->IsChildOf(InBlueprint->GeneratedClass)) {
			Child->RemoveFromRoot();
			Child->ConditionalBeginDestroy();
		}
	}
}

void FUnLuaEditorBlueprintAssetActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/) {
	for (UObject* Object : InObjects)
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>(Object))
		{
			bool bLetOpen = true;
			if (!Blueprint->SkeletonGeneratedClass || !Blueprint->GeneratedClass)
			{
				bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("FailedToLoadBlueprintWithContinue", "Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue (it can crash the editor)?"));
			}
			if (bLetOpen)
			{
				FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
				TSharedRef< IBlueprintEditor > NewKismetEditor = BlueprintEditorModule.CreateBlueprintEditor(EToolkitMode::Standalone, nullptr, Blueprint);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadBlueprint", "Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

#undef LOCTEXT_NAMESPACE