// Copyright Epic Games, Inc. All Rights Reserved.

#include "Streamer.h"
#include "Logging.h"
#include "Utils.h"
#include "UtilsFLV.h"
#include "UtilsRTMP.h"

namespace UE::RTMPixelStreaming
{
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

	UE_DISABLE_OPTIMIZATION
	void FStreamer::OnEncodedVideo(FRTMPixelStreamingEncodedVideo EncodedVideo)
	{
		UE_LOG(LogRTMPixelStreaming, VeryVerbose, TEXT("Frame Encoded\nSize: %d"), EncodedVideo.DataSize);

		uint8* buf = EncodedVideo.Data.Get();
		uint32 total = EncodedVideo.DataSize;
		uint8* buf_offset = buf;

		static uint32 ts = 0;

		while (1)
		{
			uint8* output = nullptr;
			uint32 output_len = 0;
			uint32 body_len = 0;
			uint32 offset = 0;
			uint32 nal_len = 0;

			uint8* nal = get_nal(&nal_len, &buf_offset, buf, total);
			if (nal == nullptr)
			{
				break;
			}

			if (nal[0] == 0x67)
			{
				uint32 nal_len_n = 0;
				uint8* nal_n = get_nal(&nal_len_n, &buf_offset, buf, total); // get pps
				body_len = nal_len + nal_len_n + 16;
				output_len = body_len + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
				output = new uint8[output_len];
				// flv tag header
				output[offset++] = 0x09;					// tagtype video
				output[offset++] = (uint8)(body_len >> 16); // data len
				output[offset++] = (uint8)(body_len >> 8);	// data len
				output[offset++] = (uint8)(body_len);		// data len
				output[offset++] = (uint8)(ts >> 16);		// time stamp
				output[offset++] = (uint8)(ts >> 8);		// time stamp
				output[offset++] = (uint8)(ts);				// time stamp
				output[offset++] = (uint8)(ts >> 24);		// time stamp
				output[offset++] = 0x00;					// stream id 0
				output[offset++] = 0x00;					// stream id 0
				output[offset++] = 0x00;					// stream id 0

				// flv VideoTagHeader
				output[offset++] = 0x17; // key frame, AVC
				output[offset++] = 0x00; // avc sequence header
				output[offset++] = 0x00; // composit time ??????????
				output[offset++] = 0x00; // composit time
				output[offset++] = 0x00; // composit time

				// flv VideoTagBody --AVCDecoderCOnfigurationRecord
				output[offset++] = 0x01;				  // configurationversion
				output[offset++] = nal[1];				  // avcprofileindication
				output[offset++] = nal[2];				  // profilecompatibilty
				output[offset++] = nal[3];				  // avclevelindication
				output[offset++] = 0xff;				  // reserved + lengthsizeminusone
				output[offset++] = 0xe1;				  // numofsequenceset
				output[offset++] = (uint8)(nal_len >> 8); // sequence parameter set length high 8 bits
				output[offset++] = (uint8)(nal_len);	  // sequence parameter set  length low 8 bits
				memcpy(output + offset, nal, nal_len);	  // H264 sequence parameter set
				offset += nal_len;
				output[offset++] = 0x01;					// numofpictureset
				output[offset++] = (uint8)(nal_len_n >> 8); // picture parameter set length high 8 bits
				output[offset++] = (uint8)(nal_len_n);		// picture parameter set length low 8 bits
				memcpy(output + offset, nal_n, nal_len_n);	// H264 picture parameter set

				// no need set pre_tag_size ,RTMP NO NEED
				//  flv test
				offset += nal_len_n;
				uint32_t fff = body_len + FLV_TAG_HEAD_LEN;
				output[offset++] = (uint8)(fff >> 24); // data len
				output[offset++] = (uint8)(fff >> 16); // data len
				output[offset++] = (uint8)(fff >> 8);  // data len
				output[offset++] = (uint8)(fff);	   // data len
			}
			else if (nal[0] == 0x06)
			{
			}
			else if (nal[0] == 0x65)
			{
				ts += 170;
				body_len = nal_len + 5 + 4; // flv VideoTagHeader +  NALU length
				output_len = body_len + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
				output = new uint8[output_len];
				// flv tag header
				output[offset++] = 0x09;					// tagtype video
				output[offset++] = (uint8)(body_len >> 16); // data len
				output[offset++] = (uint8)(body_len >> 8);	// data len
				output[offset++] = (uint8)(body_len);		// data len
				output[offset++] = (uint8)(ts >> 16);		// time stamp
				output[offset++] = (uint8)(ts >> 8);		// time stamp
				output[offset++] = (uint8)(ts);				// time stamp
				output[offset++] = (uint8)(ts >> 24);		// time stamp
				output[offset++] = 0x00;					// stream id 0
				output[offset++] = 0x00;					// stream id 0
				output[offset++] = 0x00;					// stream id 0

				// flv VideoTagHeader
				output[offset++] = 0x17; // key frame, AVC
				output[offset++] = 0x01; // avc NALU unit
				output[offset++] = 0x00; // composit time ??????????
				output[offset++] = 0x00; // composit time
				output[offset++] = 0x00; // composit time

				output[offset++] = (uint8)(nal_len >> 24); // nal length
				output[offset++] = (uint8)(nal_len >> 16); // nal length
				output[offset++] = (uint8)(nal_len >> 8);  // nal length
				output[offset++] = (uint8)(nal_len);	   // nal length
				memcpy(output + offset, nal, nal_len);

				// no need set pre_tag_size ,RTMP NO NEED
				offset += nal_len;
				uint32_t fff = body_len + FLV_TAG_HEAD_LEN;
				output[offset++] = (uint8)(fff >> 24); // data len
				output[offset++] = (uint8)(fff >> 16); // data len
				output[offset++] = (uint8)(fff >> 8);  // data len
				output[offset++] = (uint8)(fff);	   // data len
			}
			else if (nal[0] == 0x61)
			{
				ts += 170;
				body_len = nal_len + 5 + 4; // flv VideoTagHeader +  NALU length
				output_len = body_len + FLV_TAG_HEAD_LEN + FLV_PRE_TAG_LEN;
				output = new uint8[output_len];
				// flv tag header
				output[offset++] = 0x09;					// tagtype video
				output[offset++] = (uint8)(body_len >> 16); // data len
				output[offset++] = (uint8)(body_len >> 8);	// data len
				output[offset++] = (uint8)(body_len);		// data len
				output[offset++] = (uint8)(ts >> 16);		// time stamp
				output[offset++] = (uint8)(ts >> 8);		// time stamp
				output[offset++] = (uint8)(ts);				// time stamp
				output[offset++] = (uint8)(ts >> 24);		// time stamp
				output[offset++] = 0x00;					// stream id 0
				output[offset++] = 0x00;					// stream id 0
				output[offset++] = 0x00;					// stream id 0

				// flv VideoTagHeader
				output[offset++] = 0x27; // key frame, AVC
				output[offset++] = 0x01; // avc NALU unit
				output[offset++] = 0x00; // composit time ??????????
				output[offset++] = 0x00; // composit time
				output[offset++] = 0x00; // composit time

				output[offset++] = (uint8)(nal_len >> 24); // nal length
				output[offset++] = (uint8)(nal_len >> 16); // nal length
				output[offset++] = (uint8)(nal_len >> 8);  // nal length
				output[offset++] = (uint8)(nal_len);	   // nal length
				memcpy(output + offset, nal, nal_len);

				// no need set pre_tag_size ,RTMP NO NEED
				offset += nal_len;
				uint32_t fff = body_len + FLV_TAG_HEAD_LEN;
				output[offset++] = (uint8)(fff >> 24); // data len
				output[offset++] = (uint8)(fff >> 16); // data len
				output[offset++] = (uint8)(fff >> 8);  // data len
				output[offset++] = (uint8)(fff);	   // data len
			}

			if (RTMP_Write(rtmp, reinterpret_cast<const char*>(output), output_len) <= 0)
			{
				UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to write to server"));
			}

			delete output;
		}

		// Send additional metadata after the first frame, but only once
		static auto _ = [this, &EncodedVideo] {
			char  buf[4096];
			char* enc = buf;
			char* end = enc + sizeof(buf);

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

			size_t MetadataSize = (enc - buf);
			uint32 AVCCBufferSize = 11 + MetadataSize;
			uint8* AVCCBuffer = new uint8[AVCCBufferSize];

			size_t Offset = 0;
			AVCCBuffer[Offset++] = 0x12;
			AVCCBuffer[Offset++] = MetadataSize >> 16;
			AVCCBuffer[Offset++] = MetadataSize >> 8;
			AVCCBuffer[Offset++] = MetadataSize >> 0;
			AVCCBuffer[Offset++] = 0x0;
			AVCCBuffer[Offset++] = 0x0;
			AVCCBuffer[Offset++] = 0x0;
			AVCCBuffer[Offset++] = 0x0;
			AVCCBuffer[Offset++] = 0x0;
			AVCCBuffer[Offset++] = 0x0;
			AVCCBuffer[Offset++] = 0x0;

			memcpy(AVCCBuffer + Offset, buf, MetadataSize);

			if (RTMP_Write(rtmp, reinterpret_cast<const char*>(AVCCBuffer), AVCCBufferSize) <= 0)
			{
				UE_LOG(LogRTMPixelStreaming, Warning, TEXT("Unable to write to server"));
			}

			delete AVCCBuffer;

			return 0;
		}();
	}
	UE_ENABLE_OPTIMIZATION
} // namespace UE::RTMPixelStreaming