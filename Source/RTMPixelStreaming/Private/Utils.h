// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Async/Async.h"
#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"

namespace UE::RTMPixelStreaming
{
	template <typename T>
	inline void CommandLineParseValue(const TCHAR* Match, TAutoConsoleVariable<T>& CVar)
	{
		T Value;
		if (FParse::Value(FCommandLine::Get(), Match, Value))
			CVar->Set(Value, ECVF_SetByCommandline);
	};

	inline void CommandLineParseValue(const TCHAR* Match, TAutoConsoleVariable<FString>& CVar, bool bStopOnSeparator = false)
	{
		FString Value;
		if (FParse::Value(FCommandLine::Get(), Match, Value, bStopOnSeparator))
			CVar->Set(*Value, ECVF_SetByCommandline);
	};

	inline void CommandLineParseOption(const TCHAR* Match, TAutoConsoleVariable<bool>& CVar)
	{
		FString ValueMatch(Match);
		ValueMatch.Append(TEXT("="));
		FString Value;
		if (FParse::Value(FCommandLine::Get(), *ValueMatch, Value))
		{
			if (Value.Equals(FString(TEXT("true")), ESearchCase::IgnoreCase))
			{
				CVar->Set(true, ECVF_SetByCommandline);
			}
			else if (Value.Equals(FString(TEXT("false")), ESearchCase::IgnoreCase))
			{
				CVar->Set(false, ECVF_SetByCommandline);
			}
		}
		else if (FParse::Param(FCommandLine::Get(), Match))
		{
			CVar->Set(true, ECVF_SetByCommandline);
		}
	}

	template <typename T>
	void DoOnGameThread(T&& Func)
	{
		if (IsInGameThread())
		{
			Func();
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, [Func]() { Func(); });
		}
	}

	template <typename T>
	void DoOnGameThreadAndWait(uint32 Timeout, T&& Func)
	{
		if (IsInGameThread())
		{
			Func();
		}
		else
		{
			FEvent* TaskEvent = FPlatformProcess::GetSynchEventFromPool();
			AsyncTask(ENamedThreads::GameThread, [Func, TaskEvent]() {
				Func();
				TaskEvent->Trigger();
			});
			TaskEvent->Wait(Timeout);
			FPlatformProcess::ReturnSynchEventToPool(TaskEvent);
		}
	}

	inline bool IsLittleEndian()
	{
		int n = 1;
		return *(char*)&n == 1;
	}

	inline uint32 SwapHostToBigEndian(uint32 Value)
	{
		uint32 Temp = Value;
		if (IsLittleEndian())
		{
			Temp = (((Temp) >> 24) | (((Temp) & 0x00FF0000) >> 8) | (((Temp) & 0x0000FF00) << 8) | ((Temp) << 24));
		}

		return Temp;
	}
} // namespace UE::RTMPixelStreaming
