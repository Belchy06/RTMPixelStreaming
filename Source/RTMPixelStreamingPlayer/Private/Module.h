// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IRTMPixelStreamingPlayerModule.h"
#include "rtmp.h"

namespace UE::RTMPixelStreamingPlayer
{
	class FRTMPixelStreamingPlayerModule : public IRTMPixelStreamingPlayerModule
	{
	public:
		/** IModuleInterface implementation */
		void StartupModule() override;
		void ShutdownModule() override;
		/** End IModuleInterface implementation */

	private:
		RTMP* rtmp;
	};
} // namespace UE::RTMPixelStreaming