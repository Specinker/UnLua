
---@type UnLuaEditor
local M = UnLua.Class()

function M:Startup()
    self.ToolBarWidget = NewObject(UE.UClass.Load("/Game/Tutorials/13_EditorBlueprint/CustomToolbar.CustomToolbar_C"),self)
    self.Tab0Widget = NewObject(UE.UClass.Load("/Game/Tutorials/13_EditorBlueprint/CustomTab.CustomTab_C"),self)
    self.Tab1Widget = NewObject(UE.UClass.Load("/Game/Tutorials/13_EditorBlueprint/OpenDialog.OpenDialog_C"),self)
    self.DialogClass = UE.UClass.Load("/Game/Tutorials/13_EditorBlueprint/CustomDialog.CustomDialog_C")
    self:SetToolbarWidget(self.ToolBarWidget)
    self:TryRegisterTabWidget("Tab0",self.Tab0Widget,false);
    self:TryRegisterTabWidget("Tab1",self.Tab1Widget,false);
    self:Show(UE.FVector2D(1200,800))

    self.Tab1Widget.Button.OnClicked:Add(self,
        function()
            UE.UUnLuaEditorBPLibrary:CreateAndShowDialog(
                self.DialogClass,
                "This is Dialog Title",
                UE.FVector2D(400,300),
                true
            )
        end
    )
end

function M:Shutdown()

end

return M
