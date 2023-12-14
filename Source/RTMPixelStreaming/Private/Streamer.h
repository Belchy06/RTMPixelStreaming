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
		/** End IRTMPixelStreamingStreamer Interface */

	private:
		void OnFrameCaptured();
		void OnEncodedVideo(FRTMPixelStreamingEncodedVideo EncodedVideo);
	};
} // namespace UE::RTMPixelStreaming