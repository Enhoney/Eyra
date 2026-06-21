// Copyright Enhoney. All Rights Reserved

#pragma once
#include "Engine/DataAsset.h"

#include "EyraGameData.generated.h"

#define UE_API EYRAGAME_API

class UGameplayEffect;
class UObject;

/**
 * UEyraGameData
 *
 *	Non-mutable data asset that contains global game data.
 *  不可变的数据资产，其中包含全局游戏数据
 */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "Eyra Game Data", ShortTooltip = "Data asset containing global game data."))
class UEyraGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UE_API UEyraGameData();

	// Returns the loaded game data.
	// 获取已加载的游戏数据
	static UE_API const UEyraGameData& Get();

public:

	// Gameplay effect used to apply damage.  Uses SetByCaller for the damage magnitude.
	// 用于造成伤害的GE，类型为SetByCaller
	// 比如坠落超出高度
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	// Gameplay effect used to apply healing.  Uses SetByCaller for the healing magnitude.
	// 用于施加治疗效果的GE，治疗量根据SetByCaller
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

	// Gameplay effect used to add and remove dynamic tags.
	// 用于添加和移除动态GameplayTag的GE
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
};

#undef UE_API