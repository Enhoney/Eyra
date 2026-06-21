// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Engine/StreamableManager.h"

// 处理来自可流式句柄的进度报告
DECLARE_DELEGATE_OneParam(FEyraAssetManagerStartupJobSubstepProgress, float /*NewProgress*/);

struct FEyraAssetManagerStartupJob;

typedef TFunction<void(const FEyraAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> TEyraStartupJobFunc;

/** Handles reporting progress from streamable handles */
struct FEyraAssetManagerStartupJob
{
	// 进度代理
	FEyraAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;


	// 这个我们可以理解为一种函数指针，不过它比传统C++的函数指针更强大
	// 这是一个"能接受 FEyraAssetManagerStartupJob 和 FStreamableHandle 两个参数、返回 void 的任意可调用对象"的容器
	// 可以是普通的函数指针、Lambda表达式、有状态的Lambda、函数对象、还可以是成员函数
	TEyraStartupJobFunc JobFunc;

	// 任务名
	FString JobName;

	// 任务权重
	float JobWeight;

	// 上次更新的时间
	mutable double LastUpdate = 0;

	/** Simple job that is all synchronous */
	// 简单的同步型任务
	FEyraAssetManagerStartupJob(const FString& InJobName, const TEyraStartupJobFunc& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{
	}

	/** Perform actual loading, will return a handle if it created one */
	// 执行实际加载操作，如果创建了处理对象则会返回该处理对象的句柄
	TSharedPtr<FStreamableHandle> DoJob() const;

	// 更新进度 实际没有用到 因为资产管理器里面并没有注册复杂任务
	void UpdateSubstepProgress(float NewProgress) const
	{
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}

	// 根据流式加载句柄 实际没有用到 因为资产管理器里面并没有注册复杂任务（因为我们后面根本就没有绑定这个SubstepProgressDelegate代理）
	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		// 先判断是否绑定了
		if (SubstepProgressDelegate.IsBound())
		{
			// StreamableHandle::GetProgress traverses() a large graph and is quite expensive
			// 这个StreamableHandle::GetProgress方法会遍历一个庞大的图结构，其执行效率较低
			double Now = FPlatformTime::Seconds();
			// 比较时间，每16ms去获取一下进度并传递出去
			if (LastUpdate - Now > 1.0 / 60)
			{
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};