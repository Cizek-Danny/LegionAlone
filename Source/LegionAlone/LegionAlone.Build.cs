// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LegionAlone : ModuleRules
{
	public LegionAlone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"LegionAlone",
			"LegionAlone/Variant_Platforming",
			"LegionAlone/Variant_Platforming/Animation",
			"LegionAlone/Variant_Combat",
			"LegionAlone/Variant_Combat/AI",
			"LegionAlone/Variant_Combat/Animation",
			"LegionAlone/Variant_Combat/Gameplay",
			"LegionAlone/Variant_Combat/Interfaces",
			"LegionAlone/Variant_Combat/UI",
			"LegionAlone/Variant_SideScrolling",
			"LegionAlone/Variant_SideScrolling/AI",
			"LegionAlone/Variant_SideScrolling/Gameplay",
			"LegionAlone/Variant_SideScrolling/Interfaces",
			"LegionAlone/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
