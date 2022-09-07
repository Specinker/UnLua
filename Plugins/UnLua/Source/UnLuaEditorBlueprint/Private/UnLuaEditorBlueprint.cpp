// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnLuaEditorBlueprint.h"
#include "Modules/ModuleManager.h"

/////////////////////////////////////////////////////
// UUnLuaEditorBlueprint

UUnLuaEditorBlueprint::UUnLuaEditorBlueprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UUnLuaEditorBlueprint::SupportedByDefaultBlueprintFactory() const
{
	return false;
}

bool UUnLuaEditorBlueprint::AlwaysCompileOnLoad() const
{
	return false;
}
