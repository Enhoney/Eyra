// Copyright Enhoney. All Rights Reserved

#include "EyraLogChannels.h"

#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(LogEyraGame);

DEFINE_LOG_CATEGORY(LogEyraExperience);

DEFINE_LOG_CATEGORY(LogEyraAbilitySystem);

DEFINE_LOG_CATEGORY(LogEyraTeam);

DEFINE_LOG_CATEGORY(LogEyraGamePhase);

DEFINE_LOG_CATEGORY(LogEyraCheat);

DEFINE_LOG_CATEGORY(LogEyraGameSettingRegistry);

DEFINE_LOG_CATEGORY(LogEyraRepGraph);

FString GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			// 在 PIE模式下切换不同的游戏场景时会启用此调试辅助工具集
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[]");
}
