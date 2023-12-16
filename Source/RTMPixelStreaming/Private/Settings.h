// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"
#include "RTMPixelStreamingCodec.h"

namespace UE::RTMPixelStreaming::Settings
{
	extern void InitialiseSettings();

	extern TAutoConsoleVariable<FString> CVarRTMPixelStreamingEncoderCodec;

	ERTMPixelStreamingVideoCodec GetSelectedVideoCodec();
	bool						 GetServerUrl(FString& OutServerUrl);
	bool						 GetServerIP(FString& OutServerIP);
	bool						 GetServerPort(uint16& OutServerPort);
} // namespace UE::RTMPixelStreaming::Settings