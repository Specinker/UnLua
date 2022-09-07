// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class UCustomAsset;

class FUnLuaEditorBlueprintAssetActions : public FAssetTypeActions_Blueprint
{
public:
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
	virtual bool CanLocalize() const override { return false; }

	UObject* GetStartupInstance(UObject* InObject);

	void TryStartup(UObject* InObject);
	void TryShutdown(UObject* InObject);

	void OnHandleBlueprintPreCompile(UBlueprint* InBlueprint);

	TMap<UObject*, UObject*> MapBpToInstacne;
};

