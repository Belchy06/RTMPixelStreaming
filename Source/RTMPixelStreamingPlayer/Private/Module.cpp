// Copyright Epic Games, Inc. All Rights Reserved.

#include "Module.h"

#include "Logging.h"
#include "Misc/CoreDelegates.h"
#include "Settings.h"

namespace UE::RTMPixelStreamingPlayer
{
	void FRTMPixelStreamingPlayerModule::StartupModule()
	{
		Settings::InitialiseSettings();

		FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([this]() {
			FString ServerURL;
			FParse::Value(FCommandLine::Get(), TEXT("RTMPixelStreamingPlayerURL="), ServerURL);
			if (ServerURL.IsEmpty())
			{
				return;
			}

			rtmp = RTMP_Alloc();
			if (!rtmp)
			{
				UE_LOG(LogRTMPixelStreamingPlayer, Log, TEXT("Failed to allocate RTMP object"));
				return;
			}

			RTMP_Init(rtmp);

			RTMP_LogSetLevel(RTMP_LOGINFO);

			RTMP_LogSetCallback(RedirectRTMPLogsToUnreal);

			char* RawURL = TCHAR_TO_ANSI(*ServerURL);
			RTMP_SetupURL(rtmp, RawURL);
		});
	}

	void FRTMPixelStreamingPlayerModule::ShutdownModule()
	{
		if (!rtmp)
		{
			return;
		}

		RTMP_Close(&rtmp);

		RTMP_Free(rtmp);
	}

} // namespace UE::RTMPixelStreamingPlayer

IMPLEMENT_MODULE(UE::RTMPixelStreaming::FRTMPixelStreamingPlayerModule, RTMPixelStreamingPlayer)
