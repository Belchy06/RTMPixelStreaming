// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingVideoEncoder.h"
#include "RTMPixelStreamingCodec.h"

namespace UE::RTMPixelStreaming
{
	class FVideoEncoderSoftware : public IRTMPixelStreamingVideoEncoder
	{
	public:
		FVideoEncoderSoftware(ERTMPixelStreamingVideoCodec Codec);
		virtual ~FVideoEncoderSoftware() override = default;

		virtual void Encode(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) override;
	};
} // namespace UE::RTMPixelStreaming