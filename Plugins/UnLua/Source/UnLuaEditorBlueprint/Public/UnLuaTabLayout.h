// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Blueprint for editor utilities
 */

#pragma once

#include "UObject/ObjectMacros.h"
#include "UnLuaTabLayout.generated.h"

UCLASS(Abstract, EditInlineNew, CollapseCategories)
class UNLUAEDITORBLUEPRINT_API UUnLuaTabLayoutNode_Base: public UObject
{
	GENERATED_BODY()
};


UCLASS(meta = (DisplayName = "Stack"))
class UNLUAEDITORBLUEPRINT_API UUnLuaTabLayoutNode_Stack: public UUnLuaTabLayoutNode_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName ForegroundTabId;

	UPROPERTY(EditAnywhere)
	float SizeCoefficient = 1.0f;

	UPROPERTY(EditAnywhere)
	bool HideTabWell;

	UPROPERTY(EditAnywhere)
	TArray<FName> TabIds;
};

UCLASS(meta = (DisplayName = "Splitter"))
class UNLUAEDITORBLUEPRINT_API UUnLuaTabLayoutNode_Splitter : public UUnLuaTabLayoutNode_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EOrientation> Orientation;

	UPROPERTY(EditAnywhere)
	float SizeCoefficient = 1.0f;

	UPROPERTY(EditAnywhere,Instanced, NoClear)
	TArray<UUnLuaTabLayoutNode_Base*> Nodes;
};

USTRUCT(Blueprintable, meta = (ShowWorldContextPin))
struct UNLUAEDITORBLUEPRINT_API FUnLuaTabLayout
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Instanced, NoClear)
	TArray<UUnLuaTabLayoutNode_Base*> Nodes;

	TSharedPtr<FTabManager::FLayout> BuildTabLayout();
};
