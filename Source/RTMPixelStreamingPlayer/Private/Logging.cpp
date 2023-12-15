// Copyright Epic Games, Inc. All Rights Reserved.

#include "Logging.h"

DEFINE_LOG_CATEGORY(LogRTMPixelStreamingPlayer);

namespace UE::RTMPixelStreamingPlayer
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

		if (LogRTMPixelStreamingPlayer.IsSuppressed(RTMPToUnrealLogCategoryMap[Level]))
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
				UE_LOG(LogRTMPixelStreamingPlayer, Error, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGWARNING:
			{
				UE_LOG(LogRTMPixelStreamingPlayer, Warning, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGINFO:
			{
				UE_LOG(LogRTMPixelStreamingPlayer, Log, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGDEBUG:
			{
				UE_LOG(LogRTMPixelStreamingPlayer, Verbose, TEXT("%s"), *Msg);
				break;
			}
			case RTMP_LOGDEBUG2:
			{
				UE_LOG(LogRTMPixelStreamingPlayer, VeryVerbose, TEXT("%s"), *Msg);
				break;
			}
		}
	}
} // namespace UE::RTMPixelStreaming