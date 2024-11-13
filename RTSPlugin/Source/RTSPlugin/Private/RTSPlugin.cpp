// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "RTSPluginLog.h"
//#include "ScriptObjectReferencer.h"
#include "IRTSPlugin.h"
#include "UObject/UnrealType.h"

DEFINE_LOG_CATEGORY(LogRTSPlugin);

class FRTSPlugin : public IRTSPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FRTSPlugin, RTSPlugin)


void FRTSPlugin::StartupModule()
{
	//FScriptObjectReferencer::Init();
}

void FRTSPlugin::ShutdownModule()
{
	//FScriptObjectReferencer::Shutdown();
}
