using UnrealBuildTool;
using System.Collections.Generic;

public class EarthOnlineEditorTarget : TargetRules
{
    public EarthOnlineEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        bOverrideBuildEnvironment = true;
        ExtraModuleNames.Add("EarthOnline");
    }
}
