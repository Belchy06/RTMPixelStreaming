// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/UnrealString.h"

namespace UE::RTMPixelStreaming::Settings
{
	extern void InitialiseSettings();

	bool GetServerUrl(FString& OutServerUrl);
	bool GetServerIP(FString& OutServerIP);
	bool GetServerPort(uint16& OutServerPort);
} // namespace UE::RTMPixelStreaming::Settings