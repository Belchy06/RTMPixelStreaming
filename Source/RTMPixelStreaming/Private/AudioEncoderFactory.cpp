
// Copyright Epic Games, Inc. All Rights Reserved.

#include "AudioEncoderFactory.h"

#include "AudioEncoderSoftware.h"

namespace UE::RTMPixelStreaming
{
	TSharedPtr<IRTMPixelStreamingAudioEncoder> FAudioEncoderFactory::CreateEncoder(ERTMPixelStreamingAudioCodec Codec)
	{
		switch (Codec)
		{
			case ERTMPixelStreamingAudioCodec::AAC:
				return MakeShared<FAudioEncoderSoftware>(Codec);
			case ERTMPixelStreamingAudioCodec::Invalid:
			default:
				unimplemented();
				return nullptr;
		}
	}
} // namespace UE::RTMPixelStreaming