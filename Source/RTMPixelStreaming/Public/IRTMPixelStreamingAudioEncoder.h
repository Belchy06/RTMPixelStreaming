// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

struct RTMPIXELSTREAMING_API FRTMPixelStreamingEncodedAudio
{
	TSharedPtr<uint8> Data;
	size_t			  DataSize;
	uint32			  SampleRate;
	uint8			  NumChannels;
};

class RTMPIXELSTREAMING_API IRTMPixelStreamingAudioEncoder
{
public:
	virtual ~IRTMPixelStreamingAudioEncoder() = default;

	virtual void Encode(float* AudioData, int32 NumSamples, int32 NumChannels, const int32 SampleRate) = 0;

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEncodedAudio, FRTMPixelStreamingEncodedAudio);
	FOnEncodedAudio OnEncodedAudio;
};
