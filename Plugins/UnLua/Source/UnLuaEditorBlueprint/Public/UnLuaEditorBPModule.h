// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUnLuaEditor, Log, All);
DEFINE_LOG_CATEGORY(LogUnLuaEditor);

class FUnLuaEditorBlueprintAssetActions;

class FUnLuaEditorBlueprintModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	TSharedPtr<FUnLuaEditorBlueprintAssetActions> mActions;
};
