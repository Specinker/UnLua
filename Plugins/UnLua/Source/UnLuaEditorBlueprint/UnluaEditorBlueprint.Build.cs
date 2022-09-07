// Some copyright should be here...

using UnrealBuildTool;

public class UnLuaEditorBlueprint : ModuleRules
{
	public UnLuaEditorBlueprint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "UnrealEd",
				"LevelEditor",
				"Landscape",
				"GeometryCollectionEngine",
				"BlueprintGraph",
				"UMG",
				"UMGEditor",
				"Engine",
                "Slate",
                "SlateCore",
				"InputCore",
				"Blutility",
				"ToolMenus",
				"AssetTools",
				"EditorFramework",
				"PhysicsCore",
				"Unlua",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
