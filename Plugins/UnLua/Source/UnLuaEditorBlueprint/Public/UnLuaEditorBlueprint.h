// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Blueprint for editor utilities
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Blueprint.h"
#include "EditorUtilityBlueprint.h"
#include "UnLuaEditorBlueprint.generated.h"

UCLASS(Blueprintable, meta = (ShowWorldContextPin, DisableNativeTick))
class UNLUAEDITORBLUEPRINT_API UUnLuaEditorBlueprint : public UEditorUtilityBlueprint
{
	GENERATED_UCLASS_BODY()
	// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override;
	virtual bool AlwaysCompileOnLoad() const override;
};
