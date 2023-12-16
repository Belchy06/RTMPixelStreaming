// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings.h"
#include "Utils.h"

namespace UE::RTMPixelStreaming::Settings
{
	TAutoConsoleVariable<FString> CVarRTMPixelStreamingVideoCodec(
		TEXT("RTMPixelStreaming.Video.Codec"),
		TEXT("H264"),
		TEXT("RTMPixelStreaming video codec. Supported values are `H264`, `VP8`, `VP9`"),
		ECVF_Default);

	void InitialiseSettings()
	{
		CommandLineParseValue(TEXT("RTMPixelStreamingEncoderCodec="), CVarRTMPixelStreamingVideoCodec);
	}

	ERTMPixelStreamingVideoCodec GetSelectedVideoCodec()
	{
		const FString CodecStr = CVarRTMPixelStreamingVideoCodec.GetValueOnAnyThread();
		if (CodecStr == TEXT("H264"))
		{
			return ERTMPixelStreamingVideoCodec::H264;
		}
		else if (CodecStr == TEXT("VP8"))
		{
			return ERTMPixelStreamingVideoCodec::VP8;
		}
		else if (CodecStr == TEXT("VP9"))
		{
			return ERTMPixelStreamingVideoCodec::VP9;
		}
		else
		{
			return ERTMPixelStreamingVideoCodec::H264;
		}
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