// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Engine/AssetManager.h"
#include "EyraAssetManagerStartupJob.h"
#include "Templates/SubclassOf.h"

#include "EyraAssetManager.generated.h"

#define UE_API EYRAGAME_API

class UPrimaryDataAsset;
class UEyraGameData;
class UEyraPawnData;


// 一个约定的Bundles的命名
struct FEyraBundles
{
	static const FName Equipped;
};

/**
*	UEyraAssetManager
* Game implementation of the asset manager that overrides functionality and stores game-specific types.
* It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
* This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
* 资产管理类的实现，该实现会覆盖原有功能并存储游戏特定类型的数据。
* 我们期望大多数游戏都会覆盖AssetManager，因为它提供了一个很好的地方来存放游戏特定的加载逻辑。
* 这个类是通过在DefaultEngine.ini中设置'AssetManagerClassName'来使用的。
*/
UCLASS(MinimalAPI, Config = Game)
class UEyraAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	// 我们在这里初始化PawnData
	UE_API UEyraAssetManager();

	// Return the AssetManager singleton object
	// 获取AssetManager的单例
	static UE_API UEyraAssetManager& Get();

	// 获取GameData
	UE_API const UEyraGameData& GetGameData();


	// 获取DefaultPawnData
	UE_API const UEyraPawnData* GetDefaultPawnData() const;

	// Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded.
	// 返回由 TSoftClassPtr 指向的子类。如果该资产尚未加载，则会同步进行加载操作。
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Logs all assets currently loaded and tracked by the asset manager.
	// 记录当前由资产管理器加载并跟踪的所有资产的信息。
	// 可以通过命令行调用
	static UE_API void DumpLoadedAssets();

protected:
	//~UAssetManager interface
	// 由UEngine::InitializeObjectReferences()调用
	UE_API virtual void StartInitialLoading() override;
#if WITH_EDITOR
	// 在PIE开始之前被调用，会刷新资源目录，并且可以在此处重写以实现预加载资源
	UE_API virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
	//~End of UAssetManager interface

	// 获取GameData
	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	/**
	* FPrimaryAssetType
	* A primary asset type, represented as an FName internally and implicitly convertible back and forth
	* This exists so the blueprint API can understand it's not a normal FName
	*
	* 一种主要的资产类型，内部以 FName 格式表示，并且可以自动进行双向转换
	* 此设置的存在是为了让蓝图 API 能够明白这不是一个普通的 FName 类型
	*
	*/

	UE_API UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, 
			const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

	// 同步加载资产
	static UE_API UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	// 读取命令行参数,是否应当打印资产加载的日志
	static UE_API bool ShouldLogAssetLoads();

	// Thread safe way of adding a loaded asset to keep in memory.
	// 以线程安全的方式将以加载的资源添加到内存
	UE_API void AddLoadedAsset(const UObject* Asset);
private:
	// Flushes the StartupJobs array. Processes all startup work.
	// 执行所有Job
	UE_API void DoAllStartupJobs();

	// Sets up the ability system
	// 设置AbilitySystem
	UE_API void InitializeGameplayCueManager();

	// Called periodically during loads, could be used to feed the status to a loading screen
	// 在加载过程中会定期调用此函数，可用于将状态信息传递给加载界面
	UE_API void UpdateInitialGameContentLoadPercent(float GameContentPercent);
protected:
	// Global game data asset to use.
	// 所需的全局游戏数据配置--软引用（路径）
	// 这里是通过init配置的
	// 如果配置有问题会导致崩溃
	UPROPERTY(Config)
	TSoftObjectPtr<UEyraGameData> EyraGameDataPath;

	// Pawn data used when spawning player pawns if there isn't one set ont he player state
	// 当PlayerState中未设置相关数据时，用于生成玩家Pawn的PawnData
	// 这里是通过init配置的
	// 如果配置有问题会导致崩溃
	UPROPERTY(Config)
	TSoftObjectPtr<UEyraPawnData> DefaultPawnData;

	// Loaded version of the game data
	// 已经加载的游戏数据版本
	// Transient：不序列化该属性，该属性初始化时候会被0填充
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

    // 启动时要执行的任务列表，用于跟踪启动过程的进度
	TArray<FEyraAssetManagerStartupJob> StartupJobs;

private:
	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;

};

// 获取资产的模板实现
template<typename AssetType>
AssetType* UEyraAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		/**
		* 解除软指针的引用。
		* @返回值 若此对象已不存在或延迟指针为 null，则返回 nullptr；否则返回有效的 UObject 指针。
		*
		*/
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			// 已添加至已加载资源列表。
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

// 获取资产的类的模板实现
template<typename AssetType>
TSubclassOf<AssetType> UEyraAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			// 已添加至已加载资源列表。
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}

#undef UE_API
