#include "UnLuaEditorBPModule.h"
#include "UnLuaEditorBlueprintAssetActions.h"
#include "AssetToolsModule.h"
#include "UnLuaEditorBlueprint.h"
#include "Kismet2\KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "FUUnLuaEditorBlueprintModule"

void FUnLuaEditorBlueprintModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	mActions = MakeShareable(new FUnLuaEditorBlueprintAssetActions());
	AssetTools.RegisterAssetTypeActions(mActions.ToSharedRef());
}

void FUnLuaEditorBlueprintModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(mActions.ToSharedRef());
		mActions.Reset();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnLuaEditorBlueprintModule, UUnLuaEditorBlueprint)