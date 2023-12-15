// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#define FLV_TAG_HEAD_LEN 11
#define FLV_PRE_TAG_LEN 4

namespace UE::RTMPixelStreaming
{
	static uint32_t find_start_code(uint8_t* buf, uint32_t zeros_in_startcode)
	{
		uint32_t info;
		uint32_t i;

		info = 1;
		if ((info = (buf[zeros_in_startcode] != 1) ? 0 : 1) == 0)
			return 0;

		for (i = 0; i < zeros_in_startcode; i++)
			if (buf[i] != 0)
			{
				info = 0;
				break;
			};

		return info;
	}

	static uint8_t* get_nal(uint32_t* len, uint8_t** offset, uint8_t* start, uint32_t total)
	{
		uint32_t info;
		uint8_t* q;
		uint8_t* p = *offset;
		*len = 0;

		if ((p - start) >= total)
			return NULL;

		while (1)
		{
			info = find_start_code(p, 3);
			if (info == 1)
				break;
			p++;
			if ((p - start) >= total)
				return NULL;
		}
		q = p + 4;
		p = q;
		while (1)
		{
			info = find_start_code(p, 3);
			if (info == 1)
				break;
			p++;
			if ((p - start) >= total)
				// return NULL;
				break;
		}

		*len = (p - q);
		*offset = p;
		return q;
	}
} // namespace UE::RTMPixelStreaming