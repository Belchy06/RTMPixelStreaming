// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4

namespace UE::RTMPixelStreaming
{
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

	static uint32 BuildFLVTagHeader(uint8** Buffer, uint32 Offset, uint32 BodyLength)
	{
		uint32 Timestamp = (uint32)(FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()));

		(*Buffer)[Offset++] = 0x09;						 // tagtype video
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
} // namespace UE::RTMPixelStreaming