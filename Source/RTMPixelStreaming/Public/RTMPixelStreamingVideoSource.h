// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IPixelCaptureCapturerSource.h"
#include "PixelCaptureCapturerMultiFormat.h"
#include "Delegates/IDelegateInstance.h"
#include "PixelCaptureBufferFormat.h"

class RTMPIXELSTREAMING_API FRTMPixelStreamingVideoSource : public IPixelCaptureCapturerSource
{
public:
	FRTMPixelStreamingVideoSource();
	virtual ~FRTMPixelStreamingVideoSource() = default;

	void AddOutputFormat(int32 Format);

	/**
	 * Feed the Source with a new captured frame.
	 * @param SourceFrame The raw Source frame.
	 */
	virtual void OnFrame(const IPixelCaptureInputFrame& SourceFrame);

	TSharedPtr<IPixelCaptureOutputFrame> RequestFormat(int32 Format, int32 LayerIndex = -1);

	/**
	 * This is broadcast each time a frame exits the adapt process. Used to synchronize framerates with input rates.
	 * This should be called once per frame taking into consideration all the target formats and layers within the frame.
	 */
	DECLARE_MULTICAST_DELEGATE(FOnFrameCaptured);
	FOnFrameCaptured OnFrameCaptured;

private:
	int32 LastFrameWidth = -1;
	int32 LastFrameHeight = -1;

	TSharedPtr<FPixelCaptureCapturerMultiFormat> FrameCapturer;
	FDelegateHandle								 CaptureCompleteHandle;
	TSet<int32>									 PreInitFormats;

	void CreateFrameCapturer();
	void OnCaptureComplete();
};
