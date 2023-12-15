// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "rtmp.h"

namespace UE::RTMPixelStreaming
{
	static inline AVal* flv_str(AVal* out, const char* str)
	{
		out->av_val = (char*)str;
		out->av_len = (int)strlen(str);
		return out;
	}

	static inline void EncodeNumber(char** enc, char* end, const char* name,
		double val)
	{
		AVal s;
		*enc = AMF_EncodeNamedNumber(*enc, end, flv_str(&s, name), val);
	}

	static inline void EncodeBool(char** enc, char* end, const char* name,
		bool val)
	{
		AVal s;
		*enc = AMF_EncodeNamedBoolean(*enc, end, flv_str(&s, name), val);
	}

	static inline void EncodeStringValue(char** enc, char* end, const char* name,
		const char* val)
	{
		AVal s1, s2;
		*enc = AMF_EncodeNamedString(*enc, end, flv_str(&s1, name),
			flv_str(&s2, val));
	}

	static inline void EncodeString(char** enc, char* end, const char* str)
	{
		AVal s;
		*enc = AMF_EncodeString(*enc, end, flv_str(&s, str));
	}

} // namespace UE::RTMPixelStreaming
