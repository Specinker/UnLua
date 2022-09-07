#pragma once

#include "CoreMinimal.h"
#include "UnLuaTabLayout.h"
#include "Blueprint/UserWidget.h"
#include "UnLuaEditorBPLibrary.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWorldChanged);

UCLASS()
class UNLUAEDITORBLUEPRINT_API UUnLuaEditorBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Xin Yun Editor Inner Function", meta = (DefaultToSelf = "ContextObject"))
	static UWorld *GetWorldInLevelEditor();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Xin Yun Editor Inner Function", meta = (DefaultToSelf = "ContextObject", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "FoundWidgets"))
	static void GetWidgetsInCurrentWindowByClass(UObject* ContextObject, TArray<UUserWidget*>& FoundWidgets, TSubclassOf<UUserWidget> WidgetClass, bool TopLevelOnly = true);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Xin Yun Editor Inner Function", meta = (DefaultToSelf = "ContextObject", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "FoundWidgets"))
	static void GetWidgetsInCurrentWindowWithInterface(UObject* ContextObject, TArray<UUserWidget*>& FoundWidgets, TSubclassOf<UInterface> Interface, bool TopLevelOnly);

	UFUNCTION(BlueprintCallable, Category = "Xin Yun Editor Inner Function", meta = (DefaultToSelf = "ContextObject"))
	static UUserWidget* CreateAndShowDialog(UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass, FText WindowTitle = INVTEXT("Dialog"), FVector2D DialogSize = FVector2D(400, 300), bool IsModalWindow = true);

	UFUNCTION(BlueprintCallable, Category = "Xin Yun Editor Inner Function", meta = (DefaultToSelf = "Object"))
	static void BindWorldChangeDelegate(UObject* Object, FName FunctionName);
};
