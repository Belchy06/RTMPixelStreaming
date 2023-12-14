// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RTMPixelStreamingVideoSource.h"

enum class RTMPIXELSTREAMING_API ERTMPixelStreamingVideoFrameTypes : uint8
{
	VideoFrameKey,
	VideoFrameDelta
};

struct RTMPIXELSTREAMING_API FRTMPixelStreamingEncodedVideo
{
	ERTMPixelStreamingVideoFrameTypes FrameType;
	TSharedPtr<uint8>				  Data;
	size_t							  DataSize;
	int32							  Width;
	int32							  Height;
	int32							  QP;
};

class RTMPIXELSTREAMING_API IRTMPixelStreamingVideoEncoder
{
public:
	virtual ~IRTMPixelStreamingVideoEncoder() = default;

	virtual void Encode(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) = 0;

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEncodedVideo, FRTMPixelStreamingEncodedVideo);
	FOnEncodedVideo OnEncodedVideo;
};
