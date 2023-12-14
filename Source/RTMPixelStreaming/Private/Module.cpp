// Copyright Epic Games, Inc. All Rights Reserved.

#include "Module.h"

#include "Logging.h"
#include "Misc/CoreDelegates.h"
#include "Settings.h"
#include "Streamer.h"
#include "RTMPixelStreamingVideoSourceBackBuffer.h"
#include "VideoEncoderHardware.h"

namespace UE::RTMPixelStreaming
{
	void FRTMPixelStreamingModule::StartupModule()
	{
		Settings::InitialiseSettings();

		FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([this]() {
			FString DefaultStreamerName = TEXT("DefaultStreamer");
			DefaultStreamer = CreateStreamer(DefaultStreamerName);

			// Any time we create a new streamer, populate it's  server URL with whatever is on the command line
			FString ServerURL;
			if (!Settings::GetServerUrl(ServerURL))
			{
				// didnt get the startup URL for rtmp pixel streaming. Check deprecated options...
				FString ServerIP;
				uint16	ServerPort;
				if (Settings::GetServerIP(ServerIP) && Settings::GetServerPort(ServerPort))
				{
					// got both old parameters. Warn about deprecation and build the proper url.
					UE_LOG(LogRTMPixelStreaming, Warning, TEXT("RTMPixelStreamingIP and RTMPixelStreamingPort are deprecated flags. Use RTMPixelStreamingURL instead. eg. -RTMPixelStreamingURL=rtmp[t][e|s]://%s:%d"), *ServerIP, ServerPort);
					ServerURL = FString::Printf(TEXT("rtmp://%s:%d"), *ServerIP, ServerPort);
				}
			}

			if (ServerURL.IsEmpty())
			{
				return;
			}

			DefaultStreamer->SetVideoSource(FRTMPixelStreamingVideoSourceBackBuffer::Create());
			DefaultStreamer->SetVideoEncoder(MakeShared<FVideoEncoderHardware>());
			DefaultStreamer->Connect(ServerURL);
		});
	}

	void FRTMPixelStreamingModule::ShutdownModule()
	{
		DefaultStreamer.Reset();

		Streamers.Empty();
	}

	TSharedPtr<IRTMPixelStreamingStreamer> FRTMPixelStreamingModule::CreateStreamer(FString& Name)
	{
		TSharedPtr<IRTMPixelStreamingStreamer> ExistingStreamer = FindStreamer(Name);
		if (ExistingStreamer)
		{
			return ExistingStreamer;
		}

		TSharedPtr<FStreamer> NewStreamer = MakeShared<FStreamer>();
		{
			FScopeLock Lock(&StreamersCS);
			Streamers.Add(Name, NewStreamer);
		}

		return NewStreamer;
	}

	TSharedPtr<IRTMPixelStreamingStreamer> FRTMPixelStreamingModule::FindStreamer(FString& Name)
	{
		FScopeLock Lock(&StreamersCS);
		if (Streamers.Contains(Name))
		{
			return Streamers[Name].Pin();
		}
		return nullptr;
	}
} // namespace UE::RTMPixelStreaming

IMPLEMENT_MODULE(UE::RTMPixelStreaming::FRTMPixelStreamingModule, RTMPixelStreaming)
