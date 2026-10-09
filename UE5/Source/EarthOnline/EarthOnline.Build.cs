using UnrealBuildTool;

public class EarthOnline : ModuleRules
{
    public EarthOnline(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "Slate",
            "SlateCore",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Landscape"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Renderer",
            "Projects"
        });
    }
}
