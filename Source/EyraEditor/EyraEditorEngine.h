// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Editor/UnrealEdEngine.h"

#include "EyraEditorEngine.generated.h"


UCLASS()
class UEyraEditorEngine : public UUnrealEdEngine
{
	GENERATED_BODY()

public:

	UEyraEditorEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
	virtual void Start() override;
	virtual void Tick(float DeltaSeconds, bool bIdleMode) override;

	// 当编辑器进入PIE模式时，如果当前世界的WorldSettings是AEyraWorldSettings，并且它的ForceStandaloneNetMode属性为true，那么强制将PIE的网络模式设置为Standalone，并且弹出一个通知提示用户
	virtual FGameInstancePIEResult PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode,
		const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances) override;

private:
	void FirstTickSetup();

	bool bFirstTickSetup = false;
};