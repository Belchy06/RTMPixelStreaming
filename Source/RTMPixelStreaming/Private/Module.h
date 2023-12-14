// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingModule.h"
#include "rtmp.h"

namespace UE::RTMPixelStreaming
{
	class FRTMPixelStreamingModule : public IRTMPixelStreamingModule
	{
	public:
		/** IModuleInterface implementation */
		void StartupModule() override;
		void ShutdownModule() override;
		/** End IModuleInterface implementation */

		/** */
		virtual TSharedPtr<IRTMPixelStreamingStreamer> CreateStreamer(FString& Name) override;
		virtual TSharedPtr<IRTMPixelStreamingStreamer> FindStreamer(FString& Name) override;
		/** */

	private:
		mutable FCriticalSection							StreamersCS;
		TMap<FString, TWeakPtr<IRTMPixelStreamingStreamer>> Streamers;

		TSharedPtr<IRTMPixelStreamingStreamer> DefaultStreamer;
	};
} // namespace UE::RTMPixelStreaming