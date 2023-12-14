// Copyright Epic Games, Inc. All Rights Reserved.

#include "Streamer.h"
#include "Logging.h"

namespace UE::RTMPixelStreaming
{
	FStreamer::FStreamer()
		: VideoSource(nullptr)
		, VideoEncoder(nullptr)
	{
		rtmp = RTMP_Alloc();
		if (!rtmp)
		{
			UE_LOG(LogRTMPixelStreaming, Log, TEXT("Failed to allocate RTMP object"));
			return;
		}

		RTMP_Init(rtmp);

		RTMP_LogSetLevel(RTMP_LOGINFO);

		RTMP_LogSetCallback(RedirectRTMPLogsToUnreal);
	}

	FStreamer::~FStreamer()
	{
		if (!rtmp)
		{
			return;
		}

		RTMP_Free(rtmp);
	}

	bool FStreamer::Connect(FString& URL)
	{
		char* RawURL = TCHAR_TO_ANSI(*URL);
		RTMP_SetupURL(rtmp, RawURL);

		RTMP_EnableWrite(rtmp);

		// connect to server
		if (!RTMP_Connect(rtmp, NULL))
		{
			UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to connect to server"));
			return false;
		}

		return true;
	}

	void FStreamer::SetVideoSource(TSharedPtr<FRTMPixelStreamingVideoSource> InVideoSource)
	{
		VideoSource = InVideoSource;
		VideoSource->OnFrameCaptured.AddSP(AsShared(), FStreamer::OnFrameCaptured);
	}

	void FStreamer::SetVideoEncoder(TSharedPtr<IRTMPixelStreamingVideoEncoder> InVideoEncoder)
	{
		VideoEncoder = InVideoEncoder;
		VideoEncoder->OnEncodedVideo.AddSP(AsShared(), FStreamer::OnEncodedVideo);
	}

	void FStreamer::OnFrameCaptured()
	{
		if (VideoEncoder)
		{
			VideoEncoder->Encode(VideoSource);
		}
	}

	void FStreamer::OnEncodedVideo(FRTMPixelStreamingEncodedVideo EncodedVideo)
	{
		UE_LOG(LogRTMPixelStreaming, Log, TEXT("Frame Encoded\nSize: %d"), EncodedVideo.DataSize);
	}
} // namespace UE::RTMPixelStreaming