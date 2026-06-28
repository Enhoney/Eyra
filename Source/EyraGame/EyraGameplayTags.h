// Copyright Enhoney. All Rights Reserved

#pragma once

#include "NativeGameplayTags.h"

namespace EyraGameplayTags
{
	EYRAGAME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that Lyra will use
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	// 这些是将 MovementMode 枚举类型映射到与之关联的 GameplayTags（如下）
	EYRAGAME_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	EYRAGAME_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	EYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
};