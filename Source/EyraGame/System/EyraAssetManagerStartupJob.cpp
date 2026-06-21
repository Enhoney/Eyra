// Copyright Enhoney. All Rights Reserved

#include "EyraAssetManagerStartupJob.h"
#include "EyraLogChannels.h"

TSharedPtr<FStreamableHandle> FEyraAssetManagerStartupJob::DoJob() const
{
	// 记录任务开始时间
	const double JobStartTime = FPlatformTime::Seconds();

	TSharedPtr<FStreamableHandle> Handle;
	UE_LOG(LogEyraGame, Display, TEXT("Startup job \"%s\" starting"), *JobName);

	// 真正执行任务也是在这里
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		// 绑定资产的异步加载更新代理
		// 绑定一个委托函数，该函数会随着请求的更新被调用，仅在加载过程中有效，此操作会覆盖任何已绑定的委托函数
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this, &FEyraAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		// 等待执行完毕
		Handle->WaitUntilComplete(0.0f, false);
		// 取消代理
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	UE_LOG(LogEyraGame, Display, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName, FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
