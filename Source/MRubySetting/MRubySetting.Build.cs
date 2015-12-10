// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MRubySetting : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "./ThirdParty/")); }
    }

    public MRubySetting(TargetInfo Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
				"MRubySetting/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"MRubySetting/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UnrealEd"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        LoadMRubyLIB(Target);
	}

    public bool LoadMRubyLIB(TargetInfo Target)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "MRuby", "Libraries");

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "libmruby." + PlatformString + ".lib"));
        }

        if (isLibrarySupported)
        {
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "MRuby", "Includes"));
        }

        Definitions.Add(string.Format("WITH_MRUBY_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
