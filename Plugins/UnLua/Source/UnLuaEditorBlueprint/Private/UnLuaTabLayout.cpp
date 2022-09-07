// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnLuaTabLayout.h"

void BuildLayoutInteranl(TSharedRef<FTabManager::FSplitter> ParentSpiltter, const TArray<UUnLuaTabLayoutNode_Base*>& Nodes) {
	for (auto Node : Nodes) {
		if (auto SpiltterNode = Cast<UUnLuaTabLayoutNode_Splitter>(Node)) {
			auto ChildSpiltter = FTabManager::NewSplitter();
			ChildSpiltter->SetOrientation(SpiltterNode->Orientation);
			ChildSpiltter->SetSizeCoefficient(SpiltterNode->SizeCoefficient);
			BuildLayoutInteranl(ChildSpiltter, SpiltterNode->Nodes);
			ParentSpiltter->Split(ChildSpiltter);
		}
		else if (auto StackNode = Cast<UUnLuaTabLayoutNode_Stack>(Node)) {
			auto ChildStack = FTabManager::NewStack();
			ChildStack->SetSizeCoefficient(StackNode->SizeCoefficient);
			ChildStack->SetForegroundTab(StackNode->ForegroundTabId);
			ChildStack->SetHideTabWell(StackNode->HideTabWell);
			for (auto TabId : StackNode->TabIds) {
				ChildStack->AddTab(TabId, ETabState::OpenedTab);
			}
			ParentSpiltter->Split(ChildStack);
		}
	}
}

TSharedPtr<FTabManager::FLayout> FUnLuaTabLayout::BuildTabLayout()
{
	auto Layout = FTabManager::NewLayout("TabLayout");
	auto Area = FTabManager::NewPrimaryArea();
	auto Spiltter = FTabManager::NewSplitter();
	BuildLayoutInteranl(Spiltter, Nodes);
	Area->Split(Spiltter);
	Layout->AddArea(Area);
	return Layout;
}
