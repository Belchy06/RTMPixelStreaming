// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HAL/IConsoleManager.h"
#include "Logging/LogMacros.h"
#include "log.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRTMPixelStreaming, Log, All);

namespace UE::RTMPixelStreaming
{
	void RedirectRTMPLogsToUnreal(int Level, const char* Format, va_list Args);
} // namespace UE::RTMPixelStreaming