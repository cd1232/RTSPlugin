// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class RTSEditorPlugin : ModuleRules
	{
		public RTSEditorPlugin(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"EditorFramework",
					"UnrealEd",
					"AssetTools",
					"ClassViewer",
					"KismetCompiler",
					"Kismet",
					"BlueprintGraph"
					// ... add other public dependencies that you statically link with here ...
				}
				);
		}
	}
}
