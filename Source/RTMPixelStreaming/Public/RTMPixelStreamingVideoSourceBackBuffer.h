// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RTMPixelStreamingVideoSourceRHI.h"
#include "Widgets/SWindow.h"
#include "RHI.h"
#include "Delegates/IDelegateInstance.h"

/*
 * Use this if you want to send the UE backbuffer as video Source.
 */
class RTMPIXELSTREAMING_API FRTMPixelStreamingVideoSourceBackBuffer : public FRTMPixelStreamingVideoSourceRHI
{
public:
	static TSharedPtr<FRTMPixelStreamingVideoSourceBackBuffer> Create();
	virtual ~FRTMPixelStreamingVideoSourceBackBuffer();

private:
	FRTMPixelStreamingVideoSourceBackBuffer() = default;

	void OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& FrameBuffer);

	FDelegateHandle DelegateHandle;
};
