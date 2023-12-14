// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTMPixelStreamingVideoSourceRHI.h"
#include "Logging.h"
#include "PixelCaptureBufferFormat.h"
#include "PixelCaptureCapturerRHIRDG.h"
#include "PixelCaptureCapturerRHIToI420CPU.h"

TSharedPtr<FPixelCaptureCapturer> FRTMPixelStreamingVideoSourceRHI::CreateCapturer(int32 FinalFormat, float FinalScale)
{
	switch (FinalFormat)
	{
		case PixelCaptureBufferFormat::FORMAT_RHI:
		{
			return FPixelCaptureCapturerRHIRDG::Create(FinalScale);
		}
		case PixelCaptureBufferFormat::FORMAT_I420:
		{
			return FPixelCaptureCapturerRHIToI420CPU::Create(FinalScale);
		}
		default:
			UE_LOG(LogRTMPixelStreaming, Error, TEXT("Unsupported final format %d"), FinalFormat);
			return nullptr;
	}
}
