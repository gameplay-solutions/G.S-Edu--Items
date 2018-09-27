// Copyright 2018 Devlin Willis (ImmutableLambda). All Rights Reserved.

using UnrealBuildTool;

public class GSEduItems : ModuleRules
{
	public GSEduItems(ReadOnlyTargetRules Target) : base(Target)
	{			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);	
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			}
			);
	}
}
