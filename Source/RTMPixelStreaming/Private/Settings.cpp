// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings.h"
#include "Utils.h"

namespace UE::RTMPixelStreaming::Settings
{
	void InitialiseSettings()
	{
	}

	bool GetServerUrl(FString& OutServerUrl)
	{
		return FParse::Value(FCommandLine::Get(), TEXT("RTMPixelStreamingURL="), OutServerUrl);
	}

	bool GetServerIP(FString& OutServerIP)
	{
		return FParse::Value(FCommandLine::Get(), TEXT("RTMPixelStreamingIP="), OutServerIP);
	}

	bool GetServerPort(uint16& OutServerPort)
	{
		return FParse::Value(FCommandLine::Get(), TEXT("RTMPixelStreamingPort="), OutServerPort);
	}
} // namespace UE::RTMPixelStreaming::Settings