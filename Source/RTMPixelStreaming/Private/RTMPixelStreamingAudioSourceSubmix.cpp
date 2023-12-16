// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTMPixelStreamingAudioSourceSubmix.h"

TSharedPtr<FRTMPixelStreamingAudioSourceSubmix> FRTMPixelStreamingAudioSourceSubmix::Create()
{
	FAudioDeviceHandle Mixer = GEngine->GetMainAudioDevice();

	TSharedPtr<FRTMPixelStreamingAudioSourceSubmix> NewSource = TSharedPtr<FRTMPixelStreamingAudioSourceSubmix>(new FRTMPixelStreamingAudioSourceSubmix(Mixer));
	Mixer->RegisterSubmixBufferListener(NewSource.ToSharedRef(), Mixer->GetMainSubmixObject());

	return NewSource;
}

FRTMPixelStreamingAudioSourceSubmix::FRTMPixelStreamingAudioSourceSubmix(FAudioDeviceHandle InMixer)
{
	Mixer = InMixer;
}

FRTMPixelStreamingAudioSourceSubmix::~FRTMPixelStreamingAudioSourceSubmix()
{
	// Mixer->UnregisterSubmixBufferListener(AsShared(), Mixer->GetMainSubmixObject());
}

void FRTMPixelStreamingAudioSourceSubmix::OnNewSubmixBuffer(const USoundSubmix* OwningSubmix, float* AudioData,
	int32 NumSamples, int32 NumChannels,
	const int32 SampleRate, double AudioClock)
{
	OnAudioCaptured.Broadcast(AudioData, NumSamples, NumChannels, SampleRate);
}
