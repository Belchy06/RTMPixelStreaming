// Copyright Epic Games, Inc. All Rights Reserved.

#include "RTMPixelStreamingVideoSourceBackBuffer.h"
#include "Utils.h"
#include "PixelCaptureInputFrameRHI.h"
#include "PixelCaptureBufferFormat.h"
#include "Framework/Application/SlateApplication.h"

TSharedPtr<FRTMPixelStreamingVideoSourceBackBuffer> FRTMPixelStreamingVideoSourceBackBuffer::Create()
{
	// this was added to fix packaging
	if (!FSlateApplication::IsInitialized())
	{
		return nullptr;
	}

	TSharedPtr<FRTMPixelStreamingVideoSourceBackBuffer> NewSource = TSharedPtr<FRTMPixelStreamingVideoSourceBackBuffer>(new FRTMPixelStreamingVideoSourceBackBuffer());
	TWeakPtr<FRTMPixelStreamingVideoSourceBackBuffer>	WeakSource = NewSource;

	// Set up the callback on the game thread since FSlateApplication::Get() can only be used there
	UE::RTMPixelStreaming::DoOnGameThread([WeakSource]() {
		if (TSharedPtr<FRTMPixelStreamingVideoSourceBackBuffer> Source = WeakSource.Pin())
		{
			FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
			Source->DelegateHandle = Renderer->OnBackBufferReadyToPresent().AddSP(Source.ToSharedRef(), &FRTMPixelStreamingVideoSourceBackBuffer::OnBackBufferReady);
		}
	});

	return NewSource;
}

FRTMPixelStreamingVideoSourceBackBuffer::~FRTMPixelStreamingVideoSourceBackBuffer()
{
	if (!IsEngineExitRequested())
	{
		UE::RTMPixelStreaming::DoOnGameThread([HandleCopy = DelegateHandle]() {
			FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().Remove(HandleCopy);
		});
	}
}

void FRTMPixelStreamingVideoSourceBackBuffer::OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& FrameBuffer)
{
	OnFrame(FPixelCaptureInputFrameRHI(FrameBuffer));
}