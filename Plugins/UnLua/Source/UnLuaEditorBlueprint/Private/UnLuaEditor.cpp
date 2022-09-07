// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnLuaEditor.h"
#include "Modules/ModuleManager.h"
#include "Slate/Private/Framework/Docking/SDockingArea.h"
#include "LevelEditor.h"
#include "UnLuaModule.h"
#include "UnLua\Private\LuaDynamicBinding.h"
#include "GameDelegates.h"

UUnLuaEditor::UUnLuaEditor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, WindowTitle(FGlobalTabmanager::Get()->GetApplicationTitle())
{
}

void UUnLuaEditor::TryStartup()
{
	AddToRoot();
	if (!UnLuaModuleName.IsEmpty() && !IUnLuaModule::Get().IsActive()) {
		ULuaEnvLocator_Editor::Instance()->AddBoundObject(this, UnLuaModuleName);
		ULuaEnvLocator_Editor::Instance()->RequestRebind();
		ULuaEnvLocator_Editor::Instance()->HotReload();
	}
	Startup();
}

void UUnLuaEditor::TryShutdown()
{
	Shutdown();
	if (CurrentWindow) {
		TArray<TSharedRef<SDockTab>> AllTabs = CurrentArea->GetAllChildTabs();
		for (int32 TabIndex = 0; TabIndex < AllTabs.Num(); ++TabIndex) {
			AllTabs[TabIndex]->PersistVisualState();
			AllTabs[TabIndex]->RemoveTabFromParent();
		}
		FSlateApplication::Get().DestroyWindowImmediately(CurrentWindow.ToSharedRef());
		CurrentWindow.Reset();
		CurrentArea.Reset();
	}
	RemoveFromRoot();
	ConditionalBeginDestroy();
}

void UUnLuaEditor::TryRegisterTabWidget(FName TabId, UUserWidget* UserWidget, bool CanClose)
{
	if (UserWidget) {
		TSharedPtr<FTabManager> TabManager = GetTabManager();
		if (TabManager.IsValid()) {
			auto TabInfo = RegisteredTabIds.Find(TabId);
			if (TabInfo) {
				if (TabInfo->TabId == TabId && TabInfo->UserWidget == UserWidget && TabInfo->CanClose == CanClose) {
					return;
				}
				RegisteredTabIds.Remove(TabId);
			}
			bNeedRecreateLayout = true;
			TabManager->UnregisterTabSpawner(TabId);
			RegisteredTabIds.Add(TabId, RegisterTabInfo({ TabId, UserWidget, CanClose }));
			TabManager->RegisterTabSpawner(TabId, FOnSpawnTab::CreateLambda([UserWidget, CanClose](const FSpawnTabArgs& Args) {
				TSharedRef<SDockTab> Tab = SNew(SDockTab)
					.OnCanCloseTab_Lambda([CanClose]() { return CanClose; })
					.TabRole(ETabRole::NomadTab)
					.Content()
					[
						UserWidget->TakeWidget()
					];
				return Tab;
				}));
		}
	}
}

UWorld* UUnLuaEditor::GetWorld() const
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	if (LevelEditorModule.GetLevelEditorInstance().IsValid()) {
		return LevelEditorModule.GetLevelEditorInstance().Pin()->GetWorld();
	}
	return nullptr;
}


void UUnLuaEditor::SetToolbarWidget(UUserWidget* InToolbarWidget)
{
	ToolbarWidget = InToolbarWidget;
}

void UUnLuaEditor::Show(FVector2D WindowSize)
{
	TSharedPtr<FTabManager> TabManager = GetTabManager();
	CurrentWindow = SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(WindowSize)
		;
	if (!Layout.IsValid() || bNeedRecreateLayout) {
		Layout = TabLayout.BuildTabLayout();
		bNeedRecreateLayout = false;
	}
	auto Widget = TabManager->RestoreFrom(Layout.ToSharedRef(), TSharedPtr<SWindow>());
	CurrentWindow->SetRequestDestroyWindowOverride(FOnWindowClosed::CreateLambda([this](const TSharedRef<SWindow>& Window) {
		this->TryShutdown();
	}));

	TSharedPtr<SVerticalBox> Content = SNew(SVerticalBox);
	if (ToolbarWidget) {
		Content->AddSlot()
			.AutoHeight()
			[
				ToolbarWidget->TakeWidget()
			]
		;
	}
	Content->AddSlot()
		.FillHeight(1.0f)
		[
			Widget.ToSharedRef()
		]
	;
	CurrentArea = StaticCastSharedPtr<SDockingArea>(Widget);

	FSlateApplication::Get().AddWindowAsNativeChild(CurrentWindow.ToSharedRef(), FSlateApplication::Get().FindWidgetWindow(TabManager->GetOwnerTab().ToSharedRef()).ToSharedRef())->SetContent
	(
		Content.ToSharedRef()
	);
}

TSharedPtr<FTabManager> UUnLuaEditor::GetTabManager()
{
	if (UseLevelEditorTabManager) {
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
		return LevelEditorModule.GetLevelEditorTabManager();
	}
	else {
		return FGlobalTabmanager::Get();
	}
}
