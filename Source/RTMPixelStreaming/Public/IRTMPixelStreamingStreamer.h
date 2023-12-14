// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "rtmp.h"
#include "IRTMPixelStreamingVideoEncoder.h"
#include "RTMPixelStreamingVideoSource.h"

class RTMPIXELSTREAMING_API IRTMPixelStreamingStreamer
{
public:
	virtual ~IRTMPixelStreamingStreamer() = default;

	virtual bool Connect(FString& URL) = 0;

	virtual void SetVideoSource(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) = 0;
	virtual void SetVideoEncoder(TSharedPtr<IRTMPixelStreamingVideoEncoder> VideoEncoder) = 0;

protected:
	RTMP*									   rtmp;
	TSharedPtr<FRTMPixelStreamingVideoSource>  VideoSource;
	TSharedPtr<IRTMPixelStreamingVideoEncoder> VideoEncoder;
};