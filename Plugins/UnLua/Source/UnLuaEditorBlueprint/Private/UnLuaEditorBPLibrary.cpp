#include "UnLuaEditorBPLibrary.h"
#include "LevelEditor.h"
#include "UnLuaEditor.h"
#include "SLevelViewport.h"
#include "UnLuaEditorBPModule.h"
#include "EditorSupportDelegates.h"


UUnLuaEditorBPLibrary::UUnLuaEditorBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UUnLuaEditor* FindUnLuaEditor(UObject* InWidget) {
	if (InWidget->GetClass()->IsChildOf(UUnLuaEditor::StaticClass())) {
		return Cast<UUnLuaEditor>(InWidget);
	}
	UObject* Outer = InWidget->GetOuter();
	while (Outer) {
		if (Outer->GetClass()->IsChildOf(UUnLuaEditor::StaticClass())) {
			return Cast<UUnLuaEditor>(Outer);
		}
		Outer = Outer->GetOuter();
	}
	UE_LOG(LogUnLuaEditor, Error, TEXT("This Object is not in UnLua Editor"));
	return nullptr;
}

UWorld* UUnLuaEditorBPLibrary::GetWorldInLevelEditor()
{
	UWorld* World = nullptr;
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	World = LevelEditorModule.GetLevelEditorInstance().Pin()->GetWorld();;
	return World; 
}

void UUnLuaEditorBPLibrary::GetWidgetsInCurrentWindowByClass(UObject* Object, TArray<UUserWidget*>& FoundWidgets, TSubclassOf<UUserWidget> WidgetClass, bool TopLevelOnly)
{
	UUnLuaEditor* Editor = FindUnLuaEditor(Object);
	if (Editor) {
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr) {
			UUserWidget* LiveWidget = *Itr;
			if (!LiveWidget->IsInOuter(Editor) || !LiveWidget->GetClass()->IsChildOf(WidgetClass)) {
				continue;
			}
			if (TopLevelOnly) {
				if (LiveWidget->IsInViewport()) {
					FoundWidgets.Add(LiveWidget);
				}
			}
			else {
				FoundWidgets.Add(LiveWidget);
			}
		}
	}
}

void UUnLuaEditorBPLibrary::GetWidgetsInCurrentWindowWithInterface(UObject* Object, TArray<UUserWidget*>& FoundWidgets, TSubclassOf<UInterface> Interface, bool TopLevelOnly)
{
	UUnLuaEditor* Editor = FindUnLuaEditor(Object);
	if (Editor) {
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr) {
			UUserWidget* LiveWidget = *Itr;
			if (!LiveWidget->IsInOuter(Editor) || !LiveWidget->GetClass()->ImplementsInterface(Interface)) {
				continue;
			}
			if (TopLevelOnly) {
				if (LiveWidget->IsInViewport()) {
					FoundWidgets.Add(LiveWidget);
				}
			}
			else {
				FoundWidgets.Add(LiveWidget);
			}
		}
	}
}

UUserWidget* UUnLuaEditorBPLibrary::CreateAndShowDialog(UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass, FText WindowTitle, FVector2D DialogSize, bool IsModal)
{
	UUserWidget* UserWidget = NewObject<UUserWidget>(ContextObject, WidgetClass.Get());
	auto NewWindw = SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(DialogSize);
	if (IsModal) {
		NewWindw->SetAsModalWindow();
	}
	UUnLuaEditor* Editor = FindUnLuaEditor(ContextObject);
	if (Editor) {
		FSlateApplication::Get().AddWindowAsNativeChild(NewWindw, Editor->CurrentWindow.ToSharedRef())->SetContent
		(
			UserWidget->TakeWidget()
		);
	}
	else {
		FSlateApplication::Get().AddWindow(NewWindw)->SetContent
		(
			UserWidget->TakeWidget()
		);
	}
	return UserWidget;
}

void UUnLuaEditorBPLibrary::BindWorldChangeDelegate(UObject* Object, FName FunctionName)
{
	FEditorSupportDelegates::WorldChange.AddUFunction(Object, FunctionName);
}
