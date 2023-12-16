// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AudioDevice.h"
#include "ISubmixBufferListener.h"
#include "RTMPixelStreamingAudioSource.h"

class RTMPIXELSTREAMING_API FRTMPixelStreamingAudioSourceSubmix : public FRTMPixelStreamingAudioSource, public ISubmixBufferListener
{
public:
	static TSharedPtr<FRTMPixelStreamingAudioSourceSubmix> Create();
	virtual ~FRTMPixelStreamingAudioSourceSubmix() override;

	// ISubmixBufferListener interface
	void OnNewSubmixBuffer(const USoundSubmix* OwningSubmix, float* AudioData,
		int32 NumSamples, int32 NumChannels,
		const int32 SampleRate, double AudioClock) override;

private:
	FRTMPixelStreamingAudioSourceSubmix(FAudioDeviceHandle Mixer);

private:
	FAudioDeviceHandle Mixer;
};
