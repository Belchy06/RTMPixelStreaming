
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingAudioEncoder.h"
#include "RTMPixelStreamingCodec.h"

namespace UE::RTMPixelStreaming
{
	class FAudioEncoderFactory
	{
	public:
		static TSharedPtr<IRTMPixelStreamingAudioEncoder> CreateEncoder(ERTMPixelStreamingAudioCodec Codec);
	};
} // namespace UE::RTMPixelStreaming
