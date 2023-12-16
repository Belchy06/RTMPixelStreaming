// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RTMPixelStreamingVideoSourceRHI.h"

#include "Widgets/SWindow.h"
#include "RendererInterface.h"
#include "Delegates/IDelegateInstance.h"

/*
 * Use this if you want to send the full UE editor as video Source.
 */
class RTMPIXELSTREAMING_API FRTMPixelStreamingVideoSourceBackBufferComposited : public FRTMPixelStreamingVideoSourceRHI
{
public:
	static TSharedPtr<FRTMPixelStreamingVideoSourceBackBufferComposited> Create();
	virtual ~FRTMPixelStreamingVideoSourceBackBufferComposited();

private:
	// Our class to keep window and texture information. Use of this struct prevents SWindow propeties being updated during composition as well
	// as preventing deletion of our staging textures pre composition
	class FTexturedWindow
	{
	public:
		FTexturedWindow(FVector2D InPositionInScreen, FVector2D InSizeInScreen, float InOpacity, EWindowType InType, SWindow* InOwningWindow)
			: PositionInScreen(InPositionInScreen), SizeInScreen(InSizeInScreen), Opacity(InOpacity), Type(InType), Texture(nullptr), OwningWindow(InOwningWindow)
		{
		}

		FVector2D						   GetPositionInScreen() { return PositionInScreen; }
		FVector2D						   GetSizeInScreen() { return SizeInScreen; }
		float							   GetOpacity() { return Opacity; }
		EWindowType						   GetType() { return Type; }
		SWindow*						   GetOwningWindow() { return OwningWindow; }
		TRefCountPtr<IPooledRenderTarget>& GetTexture() { return Texture; }
		void							   SetTexture(TRefCountPtr<IPooledRenderTarget> InTexture) { Texture = InTexture; }

	private:
		FVector2D						  PositionInScreen;
		FVector2D						  SizeInScreen;
		float							  Opacity;
		EWindowType						  Type;
		TRefCountPtr<IPooledRenderTarget> Texture;
		SWindow*						  OwningWindow;
	};

private:
	FRTMPixelStreamingVideoSourceBackBufferComposited();
	void CompositeWindows();

	void OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& FrameBuffer);

	FDelegateHandle DelegateHandle;

	FCriticalSection		TopLevelWindowsCS;
	TArray<FTexturedWindow> TopLevelWindows;

private:
	// Util functions for 2D vectors
	template <class T>
	T VectorMax(const T A, const T B)
	{
		// Returns the component-wise maximum of two vectors
		return T(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y));
	}

	template <class T>
	T VectorMin(const T A, const T B)
	{
		// Returns the component-wise minimum of two vectors
		return T(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y));
	}
};