// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#define AAC_ADTS_HEADER_SIZE 7
#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4

namespace UE::RTMPixelStreaming
{
	enum class EPacketType : uint8
	{
		Audio,
		Video
	};

	static uint32 FindStartCode(uint8* Buffer, uint32 ZerosInStartCode)
	{
		uint32 Info;
		uint32 i;

		Info = 1;
		if ((Info = (Buffer[ZerosInStartCode] != 1) ? 0 : 1) == 0)
			return 0;

		for (i = 0; i < ZerosInStartCode; i++)
			if (Buffer[i] != 0)
			{
				Info = 0;
				break;
			};

		return Info;
	}

	static uint8* GetNal(uint32* Length, uint8** Offset, uint8* Start, uint32 Total)
	{
		uint32 Info;
		uint8* Q;
		uint8* P = *Offset;
		*Length = 0;

		if ((P - Start) >= Total)
			return NULL;

		while (1)
		{
			Info = FindStartCode(P, 3);
			if (Info == 1)
				break;
			P++;
			if ((P - Start) >= Total)
				return NULL;
		}
		Q = P + 4;
		P = Q;
		while (1)
		{
			Info = FindStartCode(P, 3);
			if (Info == 1)
				break;
			P++;
			if ((P - Start) >= Total)
				// return NULL;
				break;
		}

		*Length = (P - Q);
		*Offset = P;
		return Q;
	}

	static uint8* GetADTS(uint32* Length, uint8** Offset, uint8* Start, uint32 Total)
	{
		uint8* P = *Offset;
		uint32 FrameOneLength;
		uint32 FrameTwoLength;
		uint32 FrameThreeLength;
		uint32 FrameLength;

		if (Total < AAC_ADTS_HEADER_SIZE)
		{
			return nullptr;
		}
		if ((P - Start) >= Total)
		{
			return nullptr;
		}

		if (P[0] != 0xff)
		{
			return nullptr;
		}
		if ((P[1] & 0xf0) != 0xf0)
		{
			return nullptr;
		}
		FrameOneLength = P[3] & 0x03;
		FrameTwoLength = P[4];
		FrameThreeLength = (P[5] & 0xe0) >> 5;
		FrameLength = (FrameOneLength << 11) | (FrameTwoLength << 3) | FrameThreeLength;
		*Offset = P + FrameLength;
		*Length = FrameLength;
		return P;
	}

	static uint32 BuildFLVTagHeader(uint8** Buffer, uint32 Offset, uint32 BodyLength, EPacketType PacketType)
	{
		uint32 Timestamp = (uint32)(FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()));

		(*Buffer)[Offset++] = PacketType == EPacketType::Video ? 0x09 : 0x08;
		(*Buffer)[Offset++] = (uint8)(BodyLength >> 16); // data len
		(*Buffer)[Offset++] = (uint8)(BodyLength >> 8);	 // data len
		(*Buffer)[Offset++] = (uint8)(BodyLength);		 // data len
		(*Buffer)[Offset++] = (uint8)(Timestamp >> 16);	 // time stamp
		(*Buffer)[Offset++] = (uint8)(Timestamp >> 8);	 // time stamp
		(*Buffer)[Offset++] = (uint8)(Timestamp);		 // time stamp
		(*Buffer)[Offset++] = (uint8)(Timestamp >> 24);	 // time stamp
		(*Buffer)[Offset++] = 0x00;						 // stream id 0
		(*Buffer)[Offset++] = 0x00;						 // stream id 0
		(*Buffer)[Offset++] = 0x00;						 // stream id 0

		return Offset;
	}

	static uint32 BuildFLVVideoTagHeader(uint8** Buffer, uint32 Offset, bool bIsKeyframe, bool bIsSPSPPS)
	{
		(*Buffer)[Offset++] = bIsKeyframe ? 0x17 : 0x27; // key frame, AVC
		(*Buffer)[Offset++] = bIsSPSPPS ? 0x00 : 0x01;	 // avc sequence header
		(*Buffer)[Offset++] = 0x00;						 // composite time
		(*Buffer)[Offset++] = 0x00;						 // composite time
		(*Buffer)[Offset++] = 0x00;						 // composite time

		return Offset;
	}

	static uint32 BuildFLVAudioTagHeader(uint8** Buffer, uint32 Offset, uint8 NumChannels, uint8 SampleRate, bool bIsHeader)
	{
		uint8 SoundRate = 0;
		switch (SampleRate)
		{
			case 10:
			{
				// 11.025k
				SoundRate = 1;
				break;
			}
			case 7:
			{
				// 22.05k
				SoundRate = 2;
				break;
			}
			case 4:
			{
				// 44.1k
				SoundRate = 3;
				break;
			}
			default:
				checkNoEntry();
				return Offset;
		}

		(*Buffer)[Offset++] = 0xA0 | (SoundRate << 2) | 0x02 | (NumChannels - 1);
		(*Buffer)[Offset++] = bIsHeader ? 0x00 : 0x01;

		return Offset;
	}
} // namespace UE::RTMPixelStreaming