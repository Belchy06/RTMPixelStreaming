// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Audio/AudioEncoder.h"
#include "Audio/Resources/AudioResourceCPU.h"
#include "IRTMPixelStreamingAudioEncoder.h"
#include "RTMPixelStreamingCodec.h"

namespace UE::RTMPixelStreaming
{
	class FAudioEncoderSoftware : public IRTMPixelStreamingAudioEncoder
	{
	public:
		FAudioEncoderSoftware(ERTMPixelStreamingAudioCodec Codec);
		virtual ~FAudioEncoderSoftware() = default;

		virtual void Encode(float* AudioData, int32 NumSamples, int32 NumChannels, const int32 SampleRate) override;

	private:
		void UpdateConfig(int32 NumChannels, const int32 SampleRate);

	private:
		TSharedPtr<TAudioEncoder<FAudioResourceCPU>> Encoder;
	};
} // namespace UE::RTMPixelStreaming