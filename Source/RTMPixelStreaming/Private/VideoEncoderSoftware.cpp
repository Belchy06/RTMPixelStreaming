// Copyright Epic Games, Inc. All Rights Reserved.

#include "VideoEncoderSoftware.h"

#include "Logging.h"

namespace UE::RTMPixelStreaming
{
	FVideoEncoderSoftware::FVideoEncoderSoftware(ERTMPixelStreamingVideoCodec Codec)
	{
		// TODO (william.belcher)
		switch (Codec)
		{
			case ERTMPixelStreamingVideoCodec::VP8:
				unimplemented();
				break;
			case ERTMPixelStreamingVideoCodec::VP9:
				unimplemented();
				break;
			default:
				UE_LOG(LogRTMPixelStreaming, Error, TEXT("No software video encoder for target codec"));
				checkNoEntry();
		}
	}

	void FVideoEncoderSoftware::Encode(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource)
	{
	}
} // namespace UE::RTMPixelStreaming