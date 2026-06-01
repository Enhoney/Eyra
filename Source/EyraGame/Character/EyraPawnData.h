// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Engine/DataAsset.h"

#include "EyraPawnData.generated.h"

#define UE_API EYRAGAME_API

class APawn;
class UEyraAbilitySet;
class UEyraAbilityTagRelationshipMapping;
class UEyraCameraMode;
class UEyraInputConfig;
class UObject;

/****
 * UEyraPawnData
 * Pawn数据
 *
 */
 /**
  * ULyraPawnData
  *
  *	Non-mutable data asset that contains properties used to define a pawn.
  *	一种不可变的数据资产，其中包含用于定义棋子的属性。
  */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "Eyra Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class UEyraPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UE_API UEyraPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from AEyraPawn or AEyraCharacter).
	// 用于创建此兵种实例的类（通常应派生自 AEyraPawn 或 AEyraCharacter）。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Eyra|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	// 为该兵种赋予的能力组。
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Eyra|Abilities")
	// TArray<TObjectPtr<UEyraAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	// 此棋子执行行动时应采用何种能力标签的映射方式
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Eyra|Abilities")
	// TObjectPtr<UEyraAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	// 玩家控制的兵种所使用的输入配置，用于创建输入映射并绑定输入操作。
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Eyra|Input")
	// TObjectPtr<UEyraInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	// 玩家控制的兵种所采用的默认摄像机模式。
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Eyra|Camera")
	// TSubclassOf<UEyraCameraMode> DefaultCameraMode;
};


#undef UE_API
