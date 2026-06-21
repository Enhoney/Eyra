// Copyright Enhoney. All Rights Reserved

#include "EyraAssetManager.h"
#include "EyraLogChannels.h"
// #include "EyraGameplayTags.h"
#include "EyraGameData.h"
#include "AbilitySystemGlobals.h"
#include "Character/EyraPawnData.h"
#include "Misc/App.h"
#include "Stats/StatsMisc.h"
#include "Engine/Engine.h"
#include "AbilitySystem/EyraGameplayCueManager.h"
#include "Misc/ScopedSlowTask.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraAssetManager)

// 这里的宏定义看着复杂，其实就是将复杂的结构体创建过程给简化了
#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FEyraAssetManagerStartupJob(\
	#JobFunc, [this](const FEyraAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))

const FName FEyraBundles::Equipped("Equipped");

//////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("Lyra.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UEyraAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

UEyraAssetManager::UEyraAssetManager()
{
	// 这个在运行时才会去使用，所以可以初始化为空，但是GameData不行，它在引擎初始化加载的时候就得有效
	DefaultPawnData = nullptr;
}

UEyraAssetManager& UEyraAssetManager::Get()
{
	check(GEngine);

	if (UEyraAssetManager* Singleton = Cast<UEyraAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	// 致命错误，会直接崩溃
	UE_LOG(LogEyraGame, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to LyraAssetManager!"));

	// 逻辑不可达，仅保证函数完整性
	return *NewObject<UEyraAssetManager>();
}

const UEyraGameData& UEyraAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UEyraGameData>(EyraGameDataPath);
}

const UEyraPawnData* UEyraAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

void UEyraAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogEyraGame, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogEyraGame, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogEyraGame, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogEyraGame, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

void UEyraAssetManager::StartInitialLoading()
{
	// 专门用于UE启动阶段性能分析的工具，适合优化游戏启动时间
	SCOPED_BOOT_TIMING("UEyraAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	// 这个会完成所有的扫描工作，即使加载操作被延迟，现在也需要执行此操作
	Super::StartInitialLoading();

	// 申请一个Job 权重为1.f，去确认我们的GameplayCueManager是否正常，并加载需要的GameplayCue资产
	STARTUP_JOB_WEIGHTED(InitializeGameplayCueManager(), 1.f);

	{
		// Load base game data asset
		// 加载GameData资产
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
	}

	// Run all the queued up startup jobs
	DoAllStartupJobs();
}

#if WITH_EDITOR
void UEyraAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("EyraEditor", "BeginLoadingPIEData", "Loading PIE Data"));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		const UEyraGameData& LocalGameDataCommon = GetGameData();

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		// 有意安排在获取GameData之后进行，避免将GameData的处理时间计入此计时器中
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
		// 可以预先加载我们在此处所使用的Experience所需的任何内容
		// 例如：从世界设置中获取默认Experience，并结合开发人员设置的Experience覆盖内容
		// 可以参考在EditorEngine下做的操作，希望实现在编辑器下的一些快速重写操作 比如强制网络模式为单机
	}
}

#endif

// 流式加载GameData
UPrimaryDataAsset* UEyraAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, 
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("EyraEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogEyraGame, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				// 阻塞直到加载完成
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				// 这里总是可以走到的
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		// 加载失败
		UE_LOG(LogEyraGame, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), 
			*DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}

UObject* UEyraAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UEyraAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UEyraAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UEyraAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("ULyraAssetManager::DoAllStartupJobs");
	// 记录所有任务开始时间
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	/**
	* 检查一下此可执行文件是否作为DS进程启动，且不应加载仅客户端使用的数据
	* 可以通过在启动时使用"-server"参数来设置此选项为true，但是在单进程PIE模式下则为false
	* 该功能不应用于游戏或者网络用途，而硬检查“NM_DedicatedServer”选项
	*
	*/

	// 如果是运行在DS服务器上
	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		// 无需定期提供进展情况更新，直接运行这些任务即可
		for (const FEyraAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			// 总进度
			float TotalJobValue = 0.0f;
			for (const FEyraAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				// 权重相加
				TotalJobValue += StartupJob.JobWeight;
			}

			// 累计推进的进度
			float AccumulatedJobValue = 0.0f;
			for (FEyraAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				// 拿到当前任务的权重
				const float JobValue = StartupJob.JobWeight;
				// 绑定这个任务的进度更新 实际上这玩意不会被调用 我们申请的都是通过宏设置的简单任务 一下就执行完了 完全没有去调用这个代理
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
				{
					// 当前任务的进度
					const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
					// (之前已完成的任务进度权重+当前的任务已完成的权重)/总的任务权重
					const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

					This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
				});

				// 执行任务 这里会阻塞知道执行完成
				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				// 更新界面
				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}

	StartupJobs.Empty();

	UE_LOG(LogEyraGame, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UEyraAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("UEyraAssetManager::InitializeGameplayCueManager");

	// @TODO：校验GameplayCueManager是否已经初始化，如果已经初始化，就加载所有被标记为AlwaysLoad的GameplayCues
	/*UEyraGameplayCueManager* GCM = UEyraGameplayCueManager::Get();
	check(GCM);
	GCM->LoadAlawysLoadedCues();*/
}

// 这个函数实际上是用来更新资源加载进度到前台UI的，但是我们啥也没干，因为没有必要，你可自行扩展的
void UEyraAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{

}
	
