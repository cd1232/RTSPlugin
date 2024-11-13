// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "IRTSEditorPlugin.h"
#include "RTSEditorPluginLog.h"

DEFINE_LOG_CATEGORY(LogRTSEditorPlugin);

/**
* RTS blueprint module
*/
class FRTSEditorPlugin : public IRTSEditorPlugin
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FRTSEditorPlugin, RTSEditorPlugin)

void FRTSEditorPlugin::StartupModule()
{
}

void FRTSEditorPlugin::ShutdownModule()
{
}