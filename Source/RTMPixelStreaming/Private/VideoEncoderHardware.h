// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingVideoEncoder.h"

#include "Video/Encoders/Configs/VideoEncoderConfigH264.h"
#include "Video/Resources/VideoResourceRHI.h"

namespace UE::RTMPixelStreaming
{
	class FVideoEncoderHardware : public IRTMPixelStreamingVideoEncoder
	{
	public:
		FVideoEncoderHardware();
		virtual ~FVideoEncoderHardware() = default;

		virtual void Encode(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) override;

	private:
		void UpdateConfig(uint32 Width, uint32 Height);

	private:
		TSharedPtr<TVideoEncoder<FVideoResourceRHI>> Encoder;
	};
} // namespace UE::RTMPixelStreaming