// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTMPixelStreamingVideoSource.h"

#include "PixelCaptureBufferFormat.h"
#include "RTMPixelStreamingTrace.h"

FRTMPixelStreamingVideoSource::FRTMPixelStreamingVideoSource()
{
	CreateFrameCapturer();
}

void FRTMPixelStreamingVideoSource::AddOutputFormat(int32 Format)
{
	PreInitFormats.Add(Format);
	FrameCapturer->AddOutputFormat(Format);
}

void FRTMPixelStreamingVideoSource::OnFrame(const IPixelCaptureInputFrame& SourceFrame)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_ON_CHANNEL_STR("RTMPixelStreaming Video Source Frame", RTMPixelStreamingChannel);
	if (LastFrameWidth != -1 && LastFrameHeight != -1)
	{
		if (SourceFrame.GetWidth() != LastFrameWidth || SourceFrame.GetHeight() != LastFrameHeight)
		{
			CreateFrameCapturer();
		}
	}

	FrameCapturer->Capture(SourceFrame);
	LastFrameWidth = SourceFrame.GetWidth();
	LastFrameHeight = SourceFrame.GetHeight();
}

TSharedPtr<IPixelCaptureOutputFrame> FRTMPixelStreamingVideoSource::RequestFormat(int32 Format, int32 LayerIndex)
{
	if (FrameCapturer != nullptr)
	{
		return FrameCapturer->RequestFormat(Format, LayerIndex);
	}
	return nullptr;
}

void FRTMPixelStreamingVideoSource::CreateFrameCapturer()
{
	if (FrameCapturer != nullptr)
	{
		FrameCapturer->OnDisconnected();
		FrameCapturer->OnComplete.Remove(CaptureCompleteHandle);
		FrameCapturer = nullptr;
	}

	TArray<float> LayerScaling = { 1.0f };
	FrameCapturer = FPixelCaptureCapturerMultiFormat::Create(this, LayerScaling);
	CaptureCompleteHandle = FrameCapturer->OnComplete.AddRaw(this, &FRTMPixelStreamingVideoSource::OnCaptureComplete);

	for (auto& Format : PreInitFormats)
	{
		FrameCapturer->AddOutputFormat(Format);
	}
}

void FRTMPixelStreamingVideoSource::OnCaptureComplete()
{
	OnFrameCaptured.Broadcast();
}