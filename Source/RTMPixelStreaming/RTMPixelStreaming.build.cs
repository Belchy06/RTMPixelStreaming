// Copyright Epic Games, Inc. All Rights Reserved.

using System.Collections.Generic;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class RTMPixelStreaming : ModuleRules
    {
        public RTMPixelStreaming(ReadOnlyTargetRules Target) : base(Target)
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
