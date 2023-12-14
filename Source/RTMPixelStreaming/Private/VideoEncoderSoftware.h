// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingVideoEncoder.h"

namespace UE::RTMPixelStreaming
{
	class FVideoEncoderSoftware : public IRTMPixelStreamingVideoEncoder
	{
	public:
		virtual ~FVideoEncoderSoftware() override = default;

		virtual void Encode(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) override;
	};
} // namespace UE::RTMPixelStreaming