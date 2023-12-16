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
                "Engine",
                "PixelCapture",
                "TraceLog"
            });

            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "librtmp",
                "LibVpx",
                "Core",
                "Slate",
                "SlateCore",
                "RHI",
                "AVCodecsCore",
                "AVCodecsCoreRHI",
                "RenderCore",
                "Renderer"
            });
        }
    }
}
