// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HAL/IConsoleManager.h"
#include "Logging/LogMacros.h"
#include "log.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRTMPixelStreamingPlayer, Log, All);

namespace UE::RTMPixelStreamingPlayer
{
	void RedirectRTMPLogsToUnreal(int Level, const char* Format, va_list Args);
} // namespace UE::RTMPixelStreaming