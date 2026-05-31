// Copyright Enhoney. All Rights Reserved

#include "EyraEditorEngine.h"

#include "Development/EyraDeveloperSettings.h"
#include "Development/EyraPlatformEmulationSettings.h"
#include "Engine/GameInstance.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameModes/EyraWorldSettings.h"
#include "Settings/ContentBrowserSettings.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Widgets/Notifications/SNotificationList.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraEditorEngine)

#define LOCTEXT_NAMESPACE "UEyraEditorEngine"

UEyraEditorEngine::UEyraEditorEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEyraEditorEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

void UEyraEditorEngine::Start()
{
	Super::Start();
}

void UEyraEditorEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds, bIdleMode);
	
	// 第一帧执行一些设置，确保插件内容在编辑器中显示
	FirstTickSetup();
}

FGameInstancePIEResult UEyraEditorEngine::PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances)
{

	return Super::PreCreatePIEInstances(bAnyBlueprintErrors, bStartInSpectatorMode, PIEStartTime, bSupportsOnlinePIE, InNumOnlinePIEInstances);

	//if (const AEyraWorldSettings* EyraWorldSettings = Cast<AEyraWorldSettings>(EditorWorld->GetWorldSettings()))
	//{
	//	if (EyraWorldSettings->ForceStandaloneNetMode)
	//	{
	//		EPlayNetMode OutPlayNetMode;
	//		PlaySessionRequest->EditorPlaySettings->GetPlayNetMode(OutPlayNetMode);
	//		if (OutPlayNetMode != PIE_Standalone)
	//		{
	//			PlaySessionRequest->EditorPlaySettings->SetPlayNetMode(PIE_Standalone);

	//			FNotificationInfo Info(LOCTEXT("ForcingStandaloneForFrontend", "Forcing NetMode: Standalone for the Frontend"));
	//			Info.ExpireDuration = 2.0f;
	//			FSlateNotificationManager::Get().AddNotification(Info);
	//		}
	//	}
	//}

	////@TODO: Should add delegates that a *non-editor* module could bind to for PIE start/stop instead of poking directly
	//GetDefault<UEyraDeveloperSettings>()->OnPlayInEditorStarted();
	//GetDefault<UEyraPlatformEmulationSettings>()->OnPlayInEditorStarted();

	////
	//FGameInstancePIEResult Result = Super::PreCreatePIEServerInstance(bAnyBlueprintErrors, bStartInSpectatorMode, PIEStartTime, bSupportsOnlinePIE, InNumOnlinePIEInstances);

	//return Result;
}

void UEyraEditorEngine::FirstTickSetup()
{
	if (bFirstTickSetup)
	{
		return;
	}

	bFirstTickSetup = true;

	// 强制显示插件文件夹，这样插件的内容就能在编辑器中显示了
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(true);
}

#undef LOCTEXT_NAMESPACE
