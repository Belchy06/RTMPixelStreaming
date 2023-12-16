// Copyright Epic Games, Inc. All Rights Reserved.

#include "VideoEncoderHardware.h"

#include <chrono>

#include "PixelCaptureOutputFrameRHI.h"
#include "Video/Encoders/Configs/VideoEncoderConfigH264.h"

namespace UE::RTMPixelStreaming
{
	FVideoEncoderHardware::FVideoEncoderHardware()
	{
		FVideoEncoderConfigH264 EncoderConfig;

		EncoderConfig.RepeatSPSPPS = true;
		EncoderConfig.Preset = EAVPreset::UltraLowQuality;
		EncoderConfig.LatencyMode = EAVLatencyMode::UltraLowLatency;
		// We set width and height to zero here because we initialize encoder from the first frame dimensions, not this config.
		EncoderConfig.Width = 0;
		EncoderConfig.Height = 0;
		EncoderConfig.TargetFramerate = 60;
		EncoderConfig.TargetBitrate = 2500000;
		EncoderConfig.MaxBitrate = 5000000;
		EncoderConfig.MinQP = 0;
		EncoderConfig.MaxQP = 51;
		EncoderConfig.RateControlMode = ERateControlMode::CBR;
		EncoderConfig.bFillData = false;
		EncoderConfig.KeyframeInterval = 300;
		EncoderConfig.MultipassMode = EMultipassMode::Full;

		Encoder = FVideoEncoder::CreateChecked<FVideoResourceRHI>(FAVDevice::GetHardwareDevice(), EncoderConfig);
	}

	void FVideoEncoderHardware::UpdateConfig(uint32 Width, uint32 Height)
	{
		if (Encoder)
		{
			FVideoEncoderConfig	 VideoConfigMinimal = Encoder->GetMinimalConfig();
			FVideoEncoderConfig* VideoConfig = &VideoConfigMinimal;

			FVideoEncoderConfigH264& VideoConfigH264 = Encoder->GetInstance()->Edit<FVideoEncoderConfigH264>();
			VideoConfigH264.Profile = EH264Profile::Baseline;

			VideoConfig = &VideoConfigH264;

			VideoConfig->Width = Width;
			VideoConfig->Height = Height;
			VideoConfig->TargetBitrate = 2500000;
			VideoConfig->MaxBitrate = 5000000;
			VideoConfig->MinQP = 0;
			VideoConfig->MaxQP = 51;
			VideoConfig->RateControlMode = ERateControlMode::CBR;
			VideoConfig->MultipassMode = EMultipassMode::Full;
			VideoConfig->bFillData = false;

			Encoder->SetMinimalConfig(*VideoConfig);
		}
	}

	void FVideoEncoderHardware::Encode(TSharedPtr<FRTMPixelStreamingVideoSource> VideoSource)
	{
		TSharedPtr<IPixelCaptureOutputFrame> VideoFrame = VideoSource->RequestFormat(PixelCaptureBufferFormat::FORMAT_RHI);
		if (!VideoFrame)
		{
			// probably the first request which starts the adapt pipeline for this format
			return;
		}

		const FPixelCaptureOutputFrameRHI& RHIFrame = StaticCast<const FPixelCaptureOutputFrameRHI&>(*VideoFrame.Get());

		const int32 FrameWidth = VideoFrame->GetWidth();
		const int32 FrameHeight = VideoFrame->GetHeight();

		// Update the encoding config using the incoming frame resolution
		UpdateConfig(FrameWidth, FrameHeight);

		TSharedPtr<FVideoResourceRHI> VideoResource = MakeShared<FVideoResourceRHI>(Encoder->GetDevice().ToSharedRef(), FVideoResourceRHI::FRawData{ RHIFrame.GetFrameTexture(), nullptr, 0 });

		uint64_t Timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		Encoder->SendFrame(VideoResource, Timestamp, false /* bIsKeyframe */);

		FVideoPacket Packet;
		while (Encoder->ReceivePacket(Packet))
		{
			FRTMPixelStreamingEncodedVideo EncodedFrame;
			EncodedFrame.QP = Packet.QP;
			EncodedFrame.Width = FrameWidth;
			EncodedFrame.Height = FrameHeight;
			EncodedFrame.FrameType = Packet.bIsKeyframe ? ERTMPixelStreamingVideoFrameTypes::VideoFrameKey : ERTMPixelStreamingVideoFrameTypes::VideoFrameDelta;

			TSharedPtr<uint8> const CopiedData = MakeShareable(new uint8[Packet.DataSize]);
			FMemory::BigBlockMemcpy(CopiedData.Get(), Packet.DataPtr.Get(), Packet.DataSize);
			EncodedFrame.Data = CopiedData;
			EncodedFrame.DataSize = Packet.DataSize;

			OnEncodedVideo.Broadcast(EncodedFrame);
		}
	}
} // namespace UE::RTMPixelStreaming