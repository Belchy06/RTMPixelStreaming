// Copyright Epic Games, Inc. All Rights Reserved.

#include "AudioEncoderSoftware.h"

#include "Audio/Encoders/Configs/AudioEncoderConfigAAC.h"

#include <chrono>

namespace UE::RTMPixelStreaming
{
	FAudioEncoderSoftware::FAudioEncoderSoftware(ERTMPixelStreamingAudioCodec Codec)
	{
		switch (Codec)
		{
			case ERTMPixelStreamingAudioCodec::AAC:
			{
				FAudioEncoderConfigAAC EncoderConfig;
				Encoder = FAudioEncoder::CreateChecked<FAudioResourceCPU>(FAVDevice::GetHardwareDevice(), EncoderConfig);
				break;
			}
			default:
				checkNoEntry();
		}
	}

	void FAudioEncoderSoftware::Encode(float* AudioData, int32 NumSamples, int32 NumChannels, const int32 SampleRate)
	{
		UpdateConfig(NumChannels, SampleRate);

		FAVLayout const		   ResourceLayout = FAVLayout(NumSamples, 0, NumSamples);
		float				   Duration = (NumSamples / SampleRate) / NumChannels;
		FAudioDescriptor const ResourceDescriptor = FAudioDescriptor(NumSamples, Duration);

		TSharedPtr<FAudioResourceCPU> AudioResource = MakeShared<FAudioResourceCPU>(
			Encoder->GetDevice().ToSharedRef(),
			MakeShareable(new float[ResourceLayout.Size]),
			ResourceLayout,
			ResourceDescriptor);

		FMemory::Memcpy(AudioResource->GetRaw().Get(), AudioData, ResourceLayout.Size);

		uint64_t Timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		Encoder->SendFrame(AudioResource, Timestamp);

		FAudioPacket Packet;
		while (Encoder->ReceivePacket(Packet))
		{
			FRTMPixelStreamingEncodedAudio EncodedAudio;
			EncodedAudio.SampleRate = SampleRate;
			EncodedAudio.NumChannels = NumChannels;

			TSharedPtr<uint8> const CopiedData = MakeShareable(new uint8[Packet.DataSize]);
			FMemory::BigBlockMemcpy(CopiedData.Get(), Packet.DataPtr.Get(), Packet.DataSize);
			EncodedAudio.Data = CopiedData;
			EncodedAudio.DataSize = Packet.DataSize;

			OnEncodedAudio.Broadcast(EncodedAudio);
		}
	}

	void FAudioEncoderSoftware::UpdateConfig(int32 NumChannels, const int32 SampleRate)
	{
		if (Encoder)
		{
			FAudioEncoderConfig	 AudioConfigMinimal = Encoder->GetMinimalConfig();
			FAudioEncoderConfig* AudioConfig = &AudioConfigMinimal;

			AudioConfig->NumChannels = NumChannels;
			AudioConfig->Samplerate = SampleRate;

			Encoder->SetMinimalConfig(*AudioConfig);
		}
	}
} // namespace UE::RTMPixelStreaming