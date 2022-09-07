// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Blueprint for editor utilities
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Blueprint.h"
#include "UnLuaEditorBlueprintFactory.generated.h"

UCLASS()
class UNLUAEDITORBLUEPRINT_API UUUnLuaEditorBlueprintFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override { return true; }
protected:
	UPROPERTY(EditAnywhere, Category = WidgetBlueprintFactory, meta = (AllowAbstract = ""))
	TSubclassOf<class UObject> ParentClass;
};
