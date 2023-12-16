// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingVideoEncoder.h"
#include "RTMPixelStreamingCodec.h"

namespace UE::RTMPixelStreaming
{
	class FVideoEncoderFactory
	{
	public:
		static TSharedPtr<IRTMPixelStreamingVideoEncoder> CreateEncoder(ERTMPixelStreamingVideoCodec Codec);
	};
} // namespace UE::RTMPixelStreaming
