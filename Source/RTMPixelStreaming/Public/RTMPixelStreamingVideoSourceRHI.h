// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RTMPixelStreamingVideoSource.h"

/*
 * A Generic video Source for RHI frames
 */
class RTMPIXELSTREAMING_API FRTMPixelStreamingVideoSourceRHI : public FRTMPixelStreamingVideoSource
{
public:
	FRTMPixelStreamingVideoSourceRHI() = default;
	virtual ~FRTMPixelStreamingVideoSourceRHI() = default;

protected:
	virtual TSharedPtr<FPixelCaptureCapturer> CreateCapturer(int32 FinalFormat, float FinalScale) override;
};
