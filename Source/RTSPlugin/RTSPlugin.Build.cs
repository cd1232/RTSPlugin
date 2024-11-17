// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class RTSPlugin : ModuleRules
	{
		public RTSPlugin(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"SlateCore",
					"AIModule", 
					"GameplayAbilities",
					"GameplayTasks",
					"GameplayTags"
				}
				);

			if (Target.bBuildEditor == true)
			{
				PublicDependencyModuleNames.AddRange(
					new string[] 
					{
						"EditorFramework",
						"UnrealEd", 
					}
				);
			}
		}
	}
}
