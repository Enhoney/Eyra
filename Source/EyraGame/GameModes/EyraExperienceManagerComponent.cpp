// Copyright Enhoney. All Rights Reserved

#include "EyraExperienceManagerComponent.h"


#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "EyraExperienceDefinition.h"
#include "EyraExperienceActionSet.h"
#include "EyraExperienceManager.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystemSettings.h"
#include "TimerManager.h"
#include "Settings/EyraSettingsLocal.h"
#include "EyraLogChannels.h"

//@TODO：这个等后面我们自定义之后就使用我们自定义的AssetManager
#include "Engine/AssetManager.h"
#include "System/EyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraExperienceManagerComponent)

//@TODO: Async load the experience definition itself
//@TODO: 异步加载Experience本身,我们现在是在设置Experience时直接进行加载的Tryload.

//@TODO: Handle failures explicitly (go into a 'completed but failed' state rather than check()-ing)
//@待办事项：明确处理失败情况（进入“已完成但失败”的状态，而非通过检查来处理）

//@TODO: Do the action phases at the appropriate times instead of all at once
//@注意事项：应在恰当的时间执行各个行动阶段，而非一次性全部完成。

//@TODO: Support deactivating an experience and do the unloading actions
//@待办事项：支持停用体验，并执行卸载操作

//@TODO: Think about what deactivation/cleanup means for preloaded assets
//@待办事项：思考一下预加载资源的停用/清理工作意味着什么

//@TODO: Handle deactivating game features, right now we 'leak' them enabled
//@待办事项：处理游戏功能的停用问题，目前我们存在功能未停用而仍处于启用状态的情况。
// (for a client moving from experience to experience we actually want to diff the requirements and only unload some, not unload everything for them to just be immediately reloaded)
// （对于那些从一个项目转向另一个项目的客户而言，我们实际上希望对需求进行差异处理，只卸载一部分内容，而不是一次性全部卸载，以免导致他们需要重新加载所有内容）

//@TODO: Handle both built-in and URL-based plugins (search for colon?)
//@待办事项：处理内置插件和基于 URL 的插件（查找冒号？）

/** 
* 这是一个测试Experience随机延迟的命令行参数，他可以通过命令行输入读取随机延迟时间的最小值和最大值，
* 并通过这个GetExperienceLoadDelayDuration函数来获取一个随机的延迟时间，这个函数会在加载体验的过程中被调用，以模拟加载过程中的随机延迟，从而测试加载界面的表现。
* 这样，我们可以通过eyra.chaos.ExperienceDelayLoad.MinSecs 和eyra.chaos.ExperienceDelayLoad.RandomSecs这两个命令行参数来控制加载体验的随机延迟时间，从而测试加载界面的表现。
*/ 
namespace EyraConsoleVariables
{
	static float ExperienceLoadRandomDelayMin = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayMin(
		TEXT("eyra.chaos.ExperienceDelayLoad.MinSecs"),
		ExperienceLoadRandomDelayMin,
		TEXT("This value (in seconds) will be added as a delay of load completion of the experience (along with the random value lyra.chaos.ExperienceDelayLoad.RandomSecs)"),
		ECVF_Default);

	static float ExperienceLoadRandomDelayRange = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayRange(
		TEXT("eyra.chaos.ExperienceDelayLoad.RandomSecs"),
		ExperienceLoadRandomDelayRange,
		TEXT("A random amount of time between 0 and this value (in seconds) will be added as a delay of load completion of the experience (along with the fixed value eyra.chaos.ExperienceDelayLoad.MinSecs)"),
		ECVF_Default);

	float GetExperienceLoadDelayDuration()
	{
		return FMath::Max(0.0f, ExperienceLoadRandomDelayMin + FMath::FRand() * ExperienceLoadRandomDelayRange);
	}
}

UEyraExperienceManagerComponent::UEyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 开启默认网络同步
	SetIsReplicatedByDefault(true);
}

void UEyraExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// deactivate any features this experience loaded
	//@TODO: This should be handled FILO as well
	// 卸载我们之前加载的Feature
	// TODO：按照先进后出的原则进行
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		// 通过我们写的引擎子系统来确认这个插件的所有依赖已经释放完毕（引用计数归零），最终释放这个插件
		if (UEyraExperienceManager::RequestToDeactivatePlugin(PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}
	}

	//@TODO: Ensure proper handling of a partially-loaded state too
	// TODO：还需要确保对部分加载状态进行妥善处理
	if (LoadState == EEyraExperienceLoadState::Loaded)
	{
		LoadState = EEyraExperienceLoadState::Deactivating;

		// Make sure we won't complete the transition prematurely if someone registers as a pauser but fires immediately
		// 确保即便有人注册为暂停者，但随机立即执行操作，我们也不会过早完成转换过程
		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// Deactivate and unload the actions
		// 卸载这些操作
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		// 取消Action的Lambda
		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action)
				{
					Action->OnGameFeatureDeactivating(Context);
					Action->OnGameFeatureUnregistering();
				}
			}
		};

		// 取消激活Experience中的Actions
		DeactivateListOfActions(CurrentExperience->Actions);
		// 取消Experience中的ActionSets的Action
		for (const TObjectPtr<UEyraExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (ActionSet != nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();
		// 现在还不支持异步去取消这些操作，所以此处应该是0
		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogEyraExperience, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in Lyra experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

void UEyraExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, CurrentExperience);
}

bool UEyraExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != EEyraExperienceLoadState::Loaded)
	{
		OutReason = TEXT("Experience is still loading");
		return true;
	}
	else
	{
		return false;
	}
}

void UEyraExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	// @TODO：这里需要使用项目自定义的AssetManager来加载ExperienceDefinition
	// UEyraAssetManager& AssetManager = UEyraAssetManager::Get();
	UAssetManager& AssetManager = UAssetManager::Get();

	/** 获取指定主资产类型以及名称的FSoftObjectPath，若未找到会返回无效值 */
	FSoftObjectPath ExperienceDefinitionPath = AssetManager.GetPrimaryAssetPath(ExperienceId);

	/** 尝试加载该资源时，这将会调用"LoadObject"函数，该函数的执行可能会非常慢（同步加载），如果真的加载不出来，可以考虑异步加载 */
	TSubclassOf<UEyraExperienceDefinition> ExperienceDefinitionClass = Cast<UClass>(ExperienceDefinitionPath.TryLoad());

	// 必须要拿到
	check(ExperienceDefinitionClass);

	// 获取该类的CDO（Class Default Object），CDO是一个特殊的对象，它是一个类的默认实例，包含了该类的默认属性值等信息，可以通过它来访问和修改类的默认属性等信息。
	// 这里我们通过CDO来获取ExperienceDefinition，因为ExperienceDefinition本身就是一个数据资产，我们不需要创建它的实例来使用它的数据，所以直接使用CDO就可以了。
	// 如果你需要修改默认对象，应该使用GetMutableDefault而不是GetDefault，因为GetDefault返回的是一个const对象，无法修改，而GetMutableDefault返回的是一个非const对象，可以修改。
	const UEyraExperienceDefinition* ExperienceDefinition = GetDefault<UEyraExperienceDefinition>(ExperienceDefinitionClass);

	check(ExperienceDefinition != nullptr);
	check(CurrentExperience == nullptr); // 目前不支持切换体验，所以当前体验必须是空的

	// 设置当前体验，这会触发OnRep_CurrentExperience来启动加载
	CurrentExperience = ExperienceDefinition;

	// 在服务器加载
	StartExperienceLoad();
}

void UEyraExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(FOnEyraExperienceLoaded::FDelegate&& Delegate)
{
	// 如果是已经加载了就直接执行代理即可
	// 否则就存到对应优先级级别的容器中，等待加载完成后统一按照优先级顺序调用
	if (IsExperienceLoaded())
	{
		Delegate.ExecuteIfBound(CurrentExperience);
	}
	else
	{
		// 存储到高优先级的容器中（多播本质上就是个容器）
		OnExperienceLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

void UEyraExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnEyraExperienceLoaded::FDelegate&& Delegate)
{
	// 如果是已经加载了就直接执行代理即可
	// 否则就存到对应优先级级别的容器中，等待加载完成后统一按照优先级顺序调用
	if (IsExperienceLoaded())
	{
		Delegate.ExecuteIfBound(CurrentExperience);
	}
	else
	{
		// 存储到中优先级的容器中（多播本质上就是个容器）
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UEyraExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(FOnEyraExperienceLoaded::FDelegate&& Delegate)
{
	// 如果是已经加载了就直接执行代理即可
	// 否则就存到对应优先级级别的容器中，等待加载完成后统一按照优先级顺序调用
	if (IsExperienceLoaded())
	{
		Delegate.ExecuteIfBound(CurrentExperience);
	}
	else
	{
		// 存储到低优先级的容器中（多播本质上就是个容器）
		OnExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

const UEyraExperienceDefinition* UEyraExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EEyraExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

bool UEyraExperienceManagerComponent::IsExperienceLoaded() const
{
	// 判断加载状态以及体验必须存在
	return (LoadState == EEyraExperienceLoadState::Loaded && CurrentExperience != nullptr);
}

void UEyraExperienceManagerComponent::OnRep_CurrentExperience()
{
	// 从服务器同步过来的Experience，从而启动加载，这是客户端的Experience加载启动，
	// 如果是第一次设置Experience则会直接设置CurrentExperience并且调用OnRep_CurrentExperience来启动加载，
	// 如果是切换Experience则会先将CurrentExperience置空来卸载当前体验，等卸载完成后再设置CurrentExperience并调用OnRep_CurrentExperience来加载新体验
	StartExperienceLoad();
}

void UEyraExperienceManagerComponent::StartExperienceLoad()
{
	// 必须是正确的初始化状态，不能是空指针，也不能多次加载，否则就是流程错误
	check(CurrentExperience != nullptr);
	check(LoadState == EEyraExperienceLoadState::Unloaded);

	UE_LOG(LogEyraExperience, Log, TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s, %s)"),
		*CurrentExperience->GetPrimaryAssetId().ToString(), *GetClientServerContextString(this));

	// 切换到Loading状态
	LoadState = EEyraExperienceLoadState::Loading;

	// @TODO：这里需要使用项目自定义的AssetManager来加载处理的资产
	// UEyraAssetManager& AssetManager = UEyraAssetManager::Get();
	UAssetManager& AssetManager = UAssetManager::Get();

	// 需要通过Bundle来加载ExperienceDefinition中指定的资产列表，Bundles是UE中一种用于管理和组织资源加载的机制，可以将相关的资源分组到同一个Bundle中，以便于同时加载和卸载它们。
	// 他可以
	TSet<FPrimaryAssetId> BundleAssetsList;

	// 只需要加载的资产
	TSet<FSoftObjectPath> RawAssetsList;

	// 添加我们正在使用的Experience的资产
	BundleAssetsList.Add(CurrentExperience->GetPrimaryAssetId());

	// 添加这个Experience所携带的ActionSet，注意不是Actions
	for (const TObjectPtr<UEyraExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			BundleAssetsList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// 加载与该体验相关的资源

	// 需要传递这些Bundles
	TArray<FName> BundlesToLoad;

	// 这里添加了资产管理类里面的一个全局变量，他是个Bundle规则
	BundlesToLoad.Add(FEyraBundles::Equipped);

	// TODO：将此客户端/服务器相关的内容集中到"EyraAssetManager"中

	// 获取当前的网络模式，因为是挂载在GameState上面，所以可以直接通过它来获取（编辑器下都要加载）
	const ENetMode NetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (NetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (NetMode != NM_Client);

	if (bLoadClient)
	{
		/** 用于始终在客户端加载的配置/数据包 */
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}

	if (bLoadServer)
	{
		/** 用于始终在服务器加载的配置/数据包 */
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	// 一个用于同步或异步加载的句柄，只要该句柄处于激活状态，加载的资源就会保留在内存中
	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;

	if (BundleAssetsList.Num() > 0)
	{
		// 更改一组已加载的主资源的捆绑状态。
		// 等待返回的可流式请求完成，或根据需要进行轮询。
		// 如果没有需要执行的工作，返回的句柄将为null，并且会调用委托。
		// 建议使用带有Param结构的的重载版本来编写新代码。
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(
			BundleAssetsList.Array(), 
			BundlesToLoad, 
			{}, 
			false, 
			FStreamableDelegate(), 
			FStreamableManager::AsyncLoadHighPriority);
	}
	
	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;

	if (RawAssetsList.Num() > 0)
	{
		// 使用主流式管理器加载非Primary资源
		// 该操作不会自动释放句柄，如有需要请手动释放
		// 对于新代码，建议使用带有参数结构的的重载版本
		RawLoadHandle = AssetManager.LoadAssetList(
			RawAssetsList.Array(),
			FStreamableDelegate(),
			FStreamableManager::AsyncLoadHighPriority,
			TEXT("StartExperienceLoad()"));
	}

	// 如果两个异步加载操作都在进行中，则将他们合并起来
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		// 创建一个组合型句柄，该句柄会在其他句柄完成之前一直等待其完成，只要此句柄处于活动状态，相关子句柄就会保持为强引用状态
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		// 否则就选择其中一个句柄作为可用的句柄
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	// 创建一个流式加载的代理
	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// 资源已加载完成，直接调用代理函数
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		// 绑定在加载完成时执行的委托函数，仅在加载过程中有效，此操作会覆盖任何已绑定的委托函数，因此请确保在调用此函数之前绑定所有必要的委托函数。
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		// 如果处理操作被取消，则会调用此绑定的代理，仅在加载操作进行时才有效，此操作会覆盖任何已绑定的代理函数，因此请确保在调用此函数之前绑定所有必要的代理函数。（
		// PS：这里的写法感觉只是秀一下做法，直接和上面一样也是可以的）
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// 这些资源会预先加载，但我们不会因此而阻止体验的开始
	TSet<FPrimaryAssetId> PreloadAssetsList;
	// TODO：确定需要预先加载的资源（但不会进行阻塞式加载）
	if (PreloadAssetsList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(
			PreloadAssetsList.Array(),
			BundlesToLoad,
			{});
	}

}

void UEyraExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == EEyraExperienceLoadState::Loading);
	check(CurrentExperience != nullptr);

	UE_LOG(LogEyraExperience, Log, TEXT("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s, %s)"),
		*CurrentExperience->GetPrimaryAssetId().ToString(),
		*GetClientServerContextString(this));

	// 找出我们GaemFeature插件的URL——剔除重复项以及那些没有映射关系的URL
	GameFeaturePluginURLs.Reset();

	// 搜集要使用的GmaeFeature插件
	// Context作为一个上下文变量，可能会使用到，但是这里没有

	auto CollectGameFeaturePluginURLs = [this](const UPrimaryDataAsset* Context,
		const TArray<FString>& FeaturePluginList)
	{
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			// 需要对这些插件Name和URL进行验证，因为有可能写错了插件名导致找不到
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
			{
				GameFeaturePluginURLs.Add(PluginURL);
			}
			else
			{
				ensureMsgf(
					false,
					TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for experience %s - fix data, ignoring for this run"), 
					*PluginName,
					*Context->GetPrimaryAssetId().ToString());
			}
		}

		// 		// Add in our extra plugin
		// 		if (!CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent.IsEmpty())
		// 		{
		// 			FString PluginURL;
		// 			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent, PluginURL))
		// 			{
		// 				GameFeaturePluginURLs.AddUnique(PluginURL);
		// 			}
		// 		}
	};

	// 把ActionSets中每一个ActionSet对应的所有GaemFeature插件都填充进去
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);
	for (const TObjectPtr<UEyraExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			CollectGameFeaturePluginURLs(ActionSet, ActionSet->GameFeaturesToEnable);
		}
	}

	// Load and activate the features	
	// 加载并启用各项功能
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();

	if (NumGameFeaturePluginsLoading > 0)
	{
		LoadState = EEyraExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			// 增加引用计数
			UEyraExperienceManager::NotifyOfPluginActivation(PluginURL);
			// 激活该插件，在该插件激活完毕后触发是否Experience完全加载的判定
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(
				PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		// 如果没有GameFeature插件，直接调用Experience充分加载这个函数（进入下一阶段）
		OnExperienceFullLoadCompleted();
	}
}

void UEyraExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// 正在加载中的GameFeature插件数量减少
	NumGameFeaturePluginsLoading--;

	// 所有插件都加载完成后，进入Experience充分加载这个函数（进入下一阶段）
	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void UEyraExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EEyraExperienceLoadState::Loaded);

	// Insert a random delay for testing (if configured)
	// （如果已配置）插入一段随机延迟以进行测试
	if (LoadState != EEyraExperienceLoadState::LoadingChaosTestingDelay)
	{
		const float DelaySecs = EyraConsoleVariables::GetExperienceLoadDelayDuration();
		if (DelaySecs > 0.0f)
		{
			FTimerHandle DummyHandle;

			LoadState = EEyraExperienceLoadState::LoadingChaosTestingDelay;
			GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::OnExperienceFullLoadCompleted, DelaySecs, /*bLooping=*/ false);

			return;
		}
	}

	LoadState = EEyraExperienceLoadState::ExecutingActions;

	// Execute the actions
	FGameFeatureActivatingContext Context;

	// Only apply to our specific world context if set
	// 只有在设置的情况下才应用于我们的特定世界上下文
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	// 执行Action操作的Lambda，需要提供一个WorldContext
	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
	{
		for (UGameFeatureAction* Action : ActionList)
		{
			if (Action != nullptr)
			{
				// TODO: The fact that these don't take a world are potentially problematic in client-server PIE
				// The current behavior matches systems like gameplay tags where loading and registering apply to the entire process,
				// but actually applying the results to actors is restricted to a specific world
				// TODO：这些不需要一个世界的事实，在客户端-服务器 PIE 中可能会有问题，目前的行为与游戏玩法标签等系统相匹配，其中加载和注册适用于整个过程，但实际上将结果应用于Actor仅限于特定世界
				Action->OnGameFeatureRegistering();
				Action->OnGameFeatureLoading();
				Action->OnGameFeatureActivating(Context);
			}
		}
	};

	// 执行ActionSet的操作
	ActivateListOfActions(CurrentExperience->Actions);
	for (const TObjectPtr<UEyraExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	// 到这里就加载完成了，切换到Loaded状态
	LoadState = EEyraExperienceLoadState::Loaded;

	OnExperienceLoaded_HighPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_HighPriority.Clear();

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();

	// Apply any necessary scalability settings
#if !UE_SERVER
	// @TODO：这里需要去变更以下对应的用户配置，目前我们还没有编写这个类
	// UEyraSettingsLocal::Get()->OnExperienceLoaded();
#endif
}

void UEyraExperienceManagerComponent::OnActionDeactivationCompleted()
{
	// 对于正在退出的Action进行计数
	check(IsInGameThread());
	++NumObservedPausers;

	// 所有Action都已观测到退出，这里应该是异步的，但是根据注释，这里目前还不支持异步的操作
	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UEyraExperienceManagerComponent::OnAllActionsDeactivated()
{
	// TODO: We actually only deactivated and didn't fully unload...
	// TODO：实际上我们只是暂时停用了，并没有卸载
	LoadState = EEyraExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
	// TODO:	GEngine->ForceGarbageCollection(true);
	// TODO：强制进行垃圾回收，参数为true，表示强制进行垃圾回收，可能会导致性能下降，但是可以保证内存的释放
}
