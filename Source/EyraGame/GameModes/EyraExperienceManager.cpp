// Copyright Enhoney. All Rights Reserved

#include "EyraExperienceManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraExperienceManager)

#if WITH_EDITOR
void UEyraExperienceManager::OnPlayInEditorBegun()
{
	// 引擎启动的时候,确保插件请求计数图是空的,并且清空它
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UEyraExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UEyraExperienceManager* ExperienceManager = GEngine->GetEngineSubsystem<UEyraExperienceManager>();
		check(ExperienceManager);

		// Track the number of requesters who activate this plugin.
		// Multiple load/activation requests are always allowed because concurrent requests are handled.
		// 记录激活此插件的请求者数量。
		// 允许多个加载/激活请求，因为会处理并发请求。
		int32& Count = ExperienceManager->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);	// 如果没有找到，Value会是一个默认值，对于int32而言就是0
		++Count;
	}
    
}

bool UEyraExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UEyraExperienceManager* ExperienceManager = GEngine->GetEngineSubsystem<UEyraExperienceManager>();
		check(ExperienceManager);
		
		// Only let the last requester to get this far deactivate the plugin
		// 只有最后一个请求者才能成功取消插件的激活
		int32& Count = ExperienceManager->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManager->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}
    
    return true;
}

#endif