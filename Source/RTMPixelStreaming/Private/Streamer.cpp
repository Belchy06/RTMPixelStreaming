// Copyright Epic Games, Inc. All Rights Reserved.

#include "Streamer.h"

#include "Logging.h"
#include "Utils.h"
#include "UtilsFLV.h"
#include "UtilsRTMP.h"
#include "Video/CodecUtils/CodecUtilsH264.h"

namespace UE::RTMPixelStreaming
{
	using namespace UE::AVCodecCore::H264;

	FStreamer::FStreamer()
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

		if (!RTMP_ConnectStream(rtmp, 0))
		{
			UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to connect to stream"));
			return false;
		}

		return true;
	}

	void FStreamer::SetVideoSource(TSharedPtr<FRTMPixelStreamingVideoSource> InVideoSource)
	{
		VideoSource = InVideoSource;
		VideoSource->OnFrameCaptured.AddSP(AsShared(), &FStreamer::OnFrameCaptured);
	}

	void FStreamer::SetVideoEncoder(TSharedPtr<IRTMPixelStreamingVideoEncoder> InVideoEncoder)
	{
		VideoEncoder = InVideoEncoder;
		VideoEncoder->OnEncodedVideo.AddSP(AsShared(), &FStreamer::OnEncodedVideo);
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
		if (!rtmp)
		{
			return;
		}

		uint8* Data = EncodedVideo.Data.Get();
		uint32 Length = EncodedVideo.DataSize;
		uint8* DataOffset = Data;

		while (true)
		{
			uint8* Output = nullptr;
			uint32 OutputLen = 0;

			uint32 NalLength = 0;
			uint8* Nal = GetNal(&NalLength, &DataOffset, Data, Length);
			if (Nal == nullptr)
			{
				break;
			}

			ENaluType NaluType = static_cast<ENaluType>(Nal[0] & 0x1F);
			switch (NaluType)
			{
				case ENaluType::SequenceParameterSet:
				{
					uint32 PPSLength = 0;
					uint8* PPS = GetNal(&PPSLength, &DataOffset, Data, Length); // get pps
					uint32 BodyLen = NalLength + PPSLength + 16;
					OutputLen = BodyLen + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
					Output = new uint8[OutputLen];
					// flv tag header
					uint32 Offset = BuildFLVTagHeader(&Output, 0, BodyLen, EPacketType::Video);

					// flv VideoTagHeader
					Offset = BuildFLVVideoTagHeader(&Output, Offset, false, true);

					// flv VideoTagBody --AVCDecoderCOnfigurationRecord
					Output[Offset++] = 0x01;					// configurationversion
					Output[Offset++] = Nal[1];					// avcprofileindication
					Output[Offset++] = Nal[2];					// profilecompatibilty
					Output[Offset++] = Nal[3];					// avclevelindication
					Output[Offset++] = 0xff;					// reserved + lengthsizeminusone
					Output[Offset++] = 0xe1;					// numofsequenceset
					Output[Offset++] = (uint8)(NalLength >> 8); // sequence parameter set length high 8 bits
					Output[Offset++] = (uint8)(NalLength);		// sequence parameter set  length low 8 bits
					memcpy(Output + Offset, Nal, NalLength);	// H264 sequence parameter set
					Offset += NalLength;
					Output[Offset++] = 0x01;					// numofpictureset
					Output[Offset++] = (uint8)(PPSLength >> 8); // picture parameter set length high 8 bits
					Output[Offset++] = (uint8)(PPSLength);		// picture parameter set length low 8 bits
					memcpy(Output + Offset, PPS, PPSLength);	// H264 picture parameter set

					// no need set pre_tag_size ,RTMP NO NEED
					//  flv test
					Offset += PPSLength;
					uint32_t fff = BodyLen + FLV_TAG_HEAD_LEN;
					Output[Offset++] = (uint8)(fff >> 24); // data len
					Output[Offset++] = (uint8)(fff >> 16); // data len
					Output[Offset++] = (uint8)(fff >> 8);  // data len
					Output[Offset++] = (uint8)(fff);	   // data len
					break;
				}
				case ENaluType::SupplementalEnhancementInformation:
				{
					// Do nothing
					break;
				}
				case ENaluType::SliceIdrPicture:
				{
					uint32 BodyLen = NalLength + 5 + 4; // flv VideoTagHeader +  NALU length
					OutputLen = BodyLen + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
					Output = new uint8[OutputLen];
					// flv tag header
					uint32 Offset = BuildFLVTagHeader(&Output, 0, BodyLen, EPacketType::Video);

					// flv VideoTagHeader
					Offset = BuildFLVVideoTagHeader(&Output, Offset, true, false);

					Output[Offset++] = (uint8)(NalLength >> 24); // Nal length
					Output[Offset++] = (uint8)(NalLength >> 16); // Nal length
					Output[Offset++] = (uint8)(NalLength >> 8);	 // Nal length
					Output[Offset++] = (uint8)(NalLength);		 // Nal length
					memcpy(Output + Offset, Nal, NalLength);

					// no need set pre_tag_size ,RTMP NO NEED
					Offset += NalLength;
					uint32_t fff = BodyLen + FLV_TAG_HEAD_LEN;
					Output[Offset++] = (uint8)(fff >> 24); // data len
					Output[Offset++] = (uint8)(fff >> 16); // data len
					Output[Offset++] = (uint8)(fff >> 8);  // data len
					Output[Offset++] = (uint8)(fff);	   // data len
					break;
				}
				case ENaluType::SliceOfNonIdrPicture:
				{
					uint32 BodyLen = NalLength + 5 + 4; // flv VideoTagHeader +  NALU length
					OutputLen = BodyLen + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
					Output = new uint8[OutputLen];
					// flv tag header
					uint32 Offset = BuildFLVTagHeader(&Output, 0, BodyLen, EPacketType::Video);

					// flv VideoTagHeader
					Offset = BuildFLVVideoTagHeader(&Output, Offset, false, false);

					Output[Offset++] = (uint8)(NalLength >> 24); // Nal length
					Output[Offset++] = (uint8)(NalLength >> 16); // Nal length
					Output[Offset++] = (uint8)(NalLength >> 8);	 // Nal length
					Output[Offset++] = (uint8)(NalLength);		 // Nal length
					memcpy(Output + Offset, Nal, NalLength);

					// no need set pre_tag_size ,RTMP NO NEED
					Offset += NalLength;
					uint32_t fff = BodyLen + FLV_TAG_HEAD_LEN;
					Output[Offset++] = (uint8)(fff >> 24); // data len
					Output[Offset++] = (uint8)(fff >> 16); // data len
					Output[Offset++] = (uint8)(fff >> 8);  // data len
					Output[Offset++] = (uint8)(fff);	   // data len
					break;
				}
			}

			if (RTMP_Write(rtmp, reinterpret_cast<const char*>(Output), OutputLen) <= 0)
			{
				UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to write to server"));
			}

			delete Output;
		}

		// Send additional metadata after the first frame, but only once
		static auto _ = [this, &EncodedVideo] {
			char  Data[4096];
			char* enc = Data;
			char* end = enc + sizeof(Data);

			EncodeString(&enc, end, "@setDataFrame");
			EncodeString(&enc, end, "onMetaData");

			*enc++ = AMF_ECMA_ARRAY;
			enc = AMF_EncodeInt32(enc, end, 7);

			EncodeNumber(&enc, end, "duration", 0.0);
			EncodeNumber(&enc, end, "fileSize", 0.0);
			EncodeNumber(&enc, end, "width", (double)EncodedVideo.Width);
			EncodeNumber(&enc, end, "height", (double)EncodedVideo.Height);
			EncodeNumber(&enc, end, "videocodecid", (double)7);
			EncodeNumber(&enc, end, "videodatarate", (double)2500);
			EncodeNumber(&enc, end, "framerate", (double)60);

			*enc++ = 0;
			*enc++ = 0;
			*enc++ = AMF_OBJECT_END;

			size_t MetadataSize = (enc - Data);
			uint32 BufferSize = 11 + MetadataSize;
			uint8* Buffer = new uint8[BufferSize];

			size_t Offset = 0;
			Buffer[Offset++] = 0x12;
			Buffer[Offset++] = MetadataSize >> 16;
			Buffer[Offset++] = MetadataSize >> 8;
			Buffer[Offset++] = MetadataSize >> 0;
			Buffer[Offset++] = 0x0;
			Buffer[Offset++] = 0x0;
			Buffer[Offset++] = 0x0;
			Buffer[Offset++] = 0x0;
			Buffer[Offset++] = 0x0;
			Buffer[Offset++] = 0x0;
			Buffer[Offset++] = 0x0;

			memcpy(Buffer + Offset, Data, MetadataSize);

			if (RTMP_Write(rtmp, reinterpret_cast<const char*>(Buffer), BufferSize) <= 0)
			{
				UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to write to server"));
			}

			delete Buffer;

			return 0;
		}();
	}

	void FStreamer::SetAudioSource(TSharedPtr<FRTMPixelStreamingAudioSource> InAudioSource)
	{
		AudioSource = InAudioSource;
		AudioSource->OnAudioCaptured.AddSP(AsShared(), &FStreamer::OnAudioCaptured);
	}

	void FStreamer::SetAudioEncoder(TSharedPtr<IRTMPixelStreamingAudioEncoder> InAudioEncoder)
	{
		AudioEncoder = InAudioEncoder;
		AudioEncoder->OnEncodedAudio.AddSP(AsShared(), &FStreamer::OnEncodedAudio);
	}

	void FStreamer::OnAudioCaptured(float* AudioData, int32 NumSamples, int32 NumChannels, const int32 SampleRate)
	{
		if (AudioEncoder)
		{
			AudioEncoder->Encode(AudioData, NumSamples, NumChannels, SampleRate);
		}
	}

	UE_DISABLE_OPTIMIZATION
	void FStreamer::OnEncodedAudio(FRTMPixelStreamingEncodedAudio EncodedAudio)
	{
		if (!rtmp)
		{
			return;
		}

		uint8*		Data = EncodedAudio.Data.Get();
		uint32		Length = EncodedAudio.DataSize;
		uint8*		DataOffset = Data;
		static bool bIsFirstPacket = true;

		while (true)
		{
			uint8* Output = nullptr;
			uint32 OutputLen = 0;

			uint32 ADTSLength = 0;
			uint8* AudioFrame = GetADTS(&ADTSLength, &DataOffset, Data, Length);
			if (AudioFrame == nullptr)
			{
				break;
			}

			if (bIsFirstPacket)
			{
				uint32 BodyLen = 2 + 2; // AudioTagHeader + AudioSpecificConfig
				OutputLen = BodyLen + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
				Output = new uint8[OutputLen];

				// flv tag header
				uint32 Offset = BuildFLVTagHeader(&Output, 0, BodyLen, EPacketType::Audio);

				// flv AudioTagHeader
				uint8 ChannelConfiguration = (AudioFrame[3] & 0xc0) >> 6;
				uint8 SampleFrequencyIndex = (AudioFrame[2] & 0x3c) >> 2;
				Offset = BuildFLVAudioTagHeader(&Output, Offset, ChannelConfiguration, SampleFrequencyIndex, true);

				// flv AudioTagBody --AudioSpecificConfig
				uint8 AudioObjectType = ((AudioFrame[2] & 0xc0) >> 6) + 1;
				Output[Offset++] = (AudioObjectType << 3) | (SampleFrequencyIndex >> 1);
				Output[Offset++] = ((SampleFrequencyIndex & 0x01) << 7) | (ChannelConfiguration << 3);

				uint32_t fff = BodyLen + FLV_TAG_HEAD_LEN;
				Output[Offset++] = (uint8_t)(fff >> 24); // data len
				Output[Offset++] = (uint8_t)(fff >> 16); // data len
				Output[Offset++] = (uint8_t)(fff >> 8);	 // data len
				Output[Offset++] = (uint8_t)(fff);		 // data len

				bIsFirstPacket = false;
			}
			else
			{
				uint32 BodyLen = 2 + ADTSLength - AAC_ADTS_HEADER_SIZE; // remove adts header + AudioTagHeader
				OutputLen = BodyLen + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
				Output = new uint8[OutputLen];
				// flv tag header
				uint32 Offset = BuildFLVTagHeader(&Output, 0, BodyLen, EPacketType::Audio);

				// flv AudioTagHeader
				uint8 ChannelConfiguration = (AudioFrame[3] & 0xc0) >> 6;
				uint8 SampleFrequencyIndex = (AudioFrame[2] & 0x3c) >> 2;
				Offset = BuildFLVAudioTagHeader(&Output, Offset, ChannelConfiguration, SampleFrequencyIndex, false);

				// flv AudioTagBody --raw aac data
				memcpy(Output + Offset, AudioFrame + AAC_ADTS_HEADER_SIZE, (ADTSLength - AAC_ADTS_HEADER_SIZE));

				uint32_t fff = BodyLen + FLV_TAG_HEAD_LEN;
				Offset += (ADTSLength - AAC_ADTS_HEADER_SIZE);
				Output[Offset++] = (uint8_t)(fff >> 24); // data len
				Output[Offset++] = (uint8_t)(fff >> 16); // data len
				Output[Offset++] = (uint8_t)(fff >> 8);	 // data len
				Output[Offset++] = (uint8_t)(fff);		 // data len
			}

			if (RTMP_Write(rtmp, reinterpret_cast<const char*>(Output), OutputLen) <= 0)
			{
				UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to write to server"));
			}

			delete Output;
		}
	}
	UE_ENABLE_OPTIMIZATION
} // namespace UE::RTMPixelStreaming