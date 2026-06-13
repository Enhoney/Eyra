// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Components/GameStateComponent.h"
#include "LoadingProcessInterface.h"
#include "EyraExperienceManagerComponent.generated.h"

#define UE_API EYRAGAME_API

/*
 * 管理体验的游戏状态组件,非常重要
 * 它在GameState的构造函数中创建,开启了网络同步的功能用来传递Experience
 *
 */
UCLASS(MinimalAPI)
class UEyraExperienceManagerComponent final : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	UE_API UEyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer);

	//~UActorComponent interface
	UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// ILoadingProcessInterface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	// ~End of ILoadingProcessInterface

};

#undef UE_API
