// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

class RTMPIXELSTREAMING_API FRTMPixelStreamingAudioSource
{
public:
	FRTMPixelStreamingAudioSource();
	virtual ~FRTMPixelStreamingAudioSource() = default;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnAudioCaptured, float*, int32, int32, const int32);
	FOnAudioCaptured OnAudioCaptured;
};
