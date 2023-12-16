// Copyright Epic Games, Inc. All Rights Reserved.

#include "VideoEncoderFactory.h"

#include "VideoEncoderHardware.h"
#include "VideoEncoderSoftware.h"

namespace UE::RTMPixelStreaming
{
	TSharedPtr<IRTMPixelStreamingVideoEncoder> FVideoEncoderFactory::CreateEncoder(ERTMPixelStreamingVideoCodec Codec)
	{
		switch (Codec)
		{
			case ERTMPixelStreamingVideoCodec::H264:
				return MakeShared<FVideoEncoderHardware>();
			case ERTMPixelStreamingVideoCodec::VP8:
			case ERTMPixelStreamingVideoCodec::VP9:
				return MakeShared<FVideoEncoderSoftware>(Codec);
			case ERTMPixelStreamingVideoCodec::Invalid:
			default:
				unimplemented();
				return nullptr;
		}
	}
} // namespace UE::RTMPixelStreaming