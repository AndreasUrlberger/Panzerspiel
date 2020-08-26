// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Panzerspiel : ModuleRules
{
	public Panzerspiel(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Niagara", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "UINavigation" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PublicIncludePaths.AddRange(new string[] {"../Plugins/Marketplace/UINavigation/Source/UINavigation/Public", "../Plugins/Marketplace/UINavigation/Source/UINavigation/Classes" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
