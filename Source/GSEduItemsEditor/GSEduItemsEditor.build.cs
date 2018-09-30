// Copyright 2018 Devlin Willis (ImmutableLambda). All Rights Reserved.

using UnrealBuildTool;

public class GSEduItemsEditor : ModuleRules
{
	public GSEduItemsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"Settings",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"GSEduItems",
		});
	}
}
