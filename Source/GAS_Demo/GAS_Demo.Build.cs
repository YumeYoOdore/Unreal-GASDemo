// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS_Demo : ModuleRules
{
	public GAS_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//Default Modules
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
		//Gameplay Ability System Modules
		PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
		//AI-related Modules
		PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "NavigationSystem" });
		//Slate
		PublicDependencyModuleNames.AddRange(new string[] { "SlateCore" });
	}
}
