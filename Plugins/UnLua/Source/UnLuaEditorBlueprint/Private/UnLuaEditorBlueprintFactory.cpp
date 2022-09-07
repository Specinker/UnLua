#include "UnLuaEditorBlueprintFactory.h"
#include "UnLuaEditorBlueprint.h"
#include "UnLuaEditor.h"
#include "Kismet2\KismetEditorUtilities.h"

UUUnLuaEditorBlueprintFactory::UUUnLuaEditorBlueprintFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	SupportedClass = UUnLuaEditorBlueprint::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
	ParentClass = UUnLuaEditor::StaticClass();
}

UObject* UUUnLuaEditorBlueprintFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(InClass->IsChildOf(UBlueprint::StaticClass()));
	if ((ParentClass == NULL) || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), (ParentClass != NULL) ? FText::FromString(ParentClass->GetName()) : NSLOCTEXT("UnrealEd", "Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), Args));
		return NULL;
	}
	UUnLuaEditorBlueprint* NewBP = CastChecked<UUnLuaEditorBlueprint>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BPTYPE_Normal, UUnLuaEditorBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), NAME_None));
	int32 NodePositionY = 0;
	FKismetEditorUtilities::AddDefaultEventNode(NewBP, NewBP->UbergraphPages[0], FName(TEXT("Startup")), UUnLuaEditor::StaticClass(), NodePositionY);
	FKismetEditorUtilities::AddDefaultEventNode(NewBP, NewBP->UbergraphPages[0], FName(TEXT("Shutdown")), UUnLuaEditor::StaticClass(), NodePositionY);
	return NewBP;
}
