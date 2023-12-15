// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class RTMPixelStreamingPlayer : ModuleRules
    {
        public RTMPixelStreamingPlayer(ReadOnlyTargetRules Target) : base(Target)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "ApplicationCore",
                "PixelCapture",
                "TraceLog",
            });

            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "librtmp",
                "Core",
                "Slate",
                "RHI",
                "AVCodecsCore",
                "AVCodecsCoreRHI",
            });
        }
    }
}
