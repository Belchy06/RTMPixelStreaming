// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "rtmp.h"

#include "IRTMPixelStreamingVideoEncoder.h"
#include "RTMPixelStreamingVideoSource.h"

#include "IRTMPixelStreamingAudioEncoder.h"
#include "RTMPixelStreamingAudioSource.h"

class RTMPIXELSTREAMING_API IRTMPixelStreamingStreamer
{
public:
	virtual ~IRTMPixelStreamingStreamer() = default;

	virtual bool Connect(FString& URL) = 0;

	virtual void SetVideoSource(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource) = 0;
	virtual void SetVideoEncoder(TSharedPtr<IRTMPixelStreamingVideoEncoder> VideoEncoder) = 0;

	virtual void SetAudioSource(TSharedPtr<FRTMPixelStreamingAudioSource> AudioSource) = 0;
	virtual void SetAudioEncoder(TSharedPtr<IRTMPixelStreamingAudioEncoder> AudioEncoder) = 0;

protected:
	RTMP* rtmp;

	TSharedPtr<FRTMPixelStreamingVideoSource>  VideoSource;
	TSharedPtr<IRTMPixelStreamingVideoEncoder> VideoEncoder;

	TSharedPtr<FRTMPixelStreamingAudioSource>  AudioSource;
	TSharedPtr<IRTMPixelStreamingAudioEncoder> AudioEncoder;
};