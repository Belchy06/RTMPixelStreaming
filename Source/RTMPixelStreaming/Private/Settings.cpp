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

	TAutoConsoleVariable<FString> CVarRTMPixelStreamingAudioCodec(
		TEXT("RTMPixelStreaming.Audio.Codec"),
		TEXT("AAC"),
		TEXT("RTMPixelStreaming audio codec. Supported values are `AAC`"),
		ECVF_Default);

	void InitialiseSettings()
	{
		CommandLineParseValue(TEXT("RTMPixelStreamingVideoCodec="), CVarRTMPixelStreamingVideoCodec);
		CommandLineParseValue(TEXT("RTMPixelStreamingAudioCodec="), CVarRTMPixelStreamingAudioCodec);
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

	ERTMPixelStreamingAudioCodec GetSelectedAudioCodec()
	{
		const FString CodecStr = CVarRTMPixelStreamingAudioCodec.GetValueOnAnyThread();
		if (CodecStr == TEXT("AAC"))
		{
			return ERTMPixelStreamingAudioCodec::AAC;
		}
		else
		{
			return ERTMPixelStreamingAudioCodec::AAC;
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