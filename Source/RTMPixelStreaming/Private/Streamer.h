// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingStreamer.h"

namespace UE::RTMPixelStreaming
{
	class FStreamer : public IRTMPixelStreamingStreamer, public TSharedFromThis<FStreamer>
	{
	public:
		FStreamer();
		virtual ~FStreamer() override;

		/** Begin IRTMPixelStreamingStreamer Interface */
		virtual bool Connect(FString& URL) override;
		virtual void SetVideoSource(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) override;
		virtual void SetVideoEncoder(TSharedPtr<IRTMPixelStreamingVideoEncoder> VideoEncoder) override;
		virtual void SetAudioSource(TSharedPtr<FRTMPixelStreamingAudioSource> AudioSource) override;
		virtual void SetAudioEncoder(TSharedPtr<IRTMPixelStreamingAudioEncoder> AudioEncoder) override;
		/** End IRTMPixelStreamingStreamer Interface */

	private:
		void OnFrameCaptured();
		void OnEncodedVideo(FRTMPixelStreamingEncodedVideo EncodedVideo);

		void OnAudioCaptured(float* AudioData, int32 NumSamples, int32 NumChannels, const int32 SampleRate);
		void OnEncodedAudio(FRTMPixelStreamingEncodedAudio EncodedAudio);
	};
} // namespace UE::RTMPixelStreaming