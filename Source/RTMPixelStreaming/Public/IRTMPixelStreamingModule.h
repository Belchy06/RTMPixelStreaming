// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "IRTMPixelStreamingStreamer.h"

/**
 * The public interface of the RTMP Pixel Streaming module.
 */
class RTMPIXELSTREAMING_API IRTMPixelStreamingModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface.
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline IRTMPixelStreamingModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IRTMPixelStreamingModule>("RTMPixelStreaming");
	}

	/**
	 * Checks to see if this module is loaded.
	 *
	 * @return True if the module is loaded.
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RTMPixelStreaming");
	}

	virtual TSharedPtr<IRTMPixelStreamingStreamer> CreateStreamer(FString& Name) = 0;

	virtual TSharedPtr<IRTMPixelStreamingStreamer> FindStreamer(FString& Name) = 0;
};
