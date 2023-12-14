// Copyright Epic Games, Inc. All Rights Reserved.

#include "Logging.h"

DEFINE_LOG_CATEGORY(LogRTMPixelStreaming);

namespace UE::RTMPixelStreaming
{
	void RedirectRTMPLogsToUnreal(int Level, const char* Format, va_list Args)
	{
		static const ELogVerbosity::Type RTMPToUnrealLogCategoryMap[] = {
			ELogVerbosity::Error,
			ELogVerbosity::Error,
			ELogVerbosity::Warning,
			ELogVerbosity::Log,
			ELogVerbosity::Verbose,
			ELogVerbosity::VeryVerbose
		};

		if (LogRTMPixelStreaming.IsSuppressed(RTMPToUnrealLogCategoryMap[Level]))
		{
			return;
		}

		char Str[2048] = "";
		vsnprintf(Str, 2048 - 1, Format, Args);

		FString Msg(Str);

		switch (Level)
		{
			case RTMP_LOGCRIT:
			case RTMP_LOGERROR:
			{
				UE_LOG(LogRTMPixelStreaming, Error, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGWARNING:
			{
				UE_LOG(LogRTMPixelStreaming, Warning, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGINFO:
			{
				UE_LOG(LogRTMPixelStreaming, Log, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGDEBUG:
			{
				UE_LOG(LogRTMPixelStreaming, Verbose, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGDEBUG2:
			{
				UE_LOG(LogRTMPixelStreaming, VeryVerbose, TEXT("%s"), *Msg);
				break;
			}
		}
	}
} // namespace UE::RTMPixelStreaming