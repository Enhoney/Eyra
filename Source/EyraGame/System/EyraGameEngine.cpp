// Copyright Enhoney. All Rights Reserved

#include "EyraGameEngine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraGameEngine)

class IEngineLoop;

UEyraGameEngine::UEyraGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEyraGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
	// 在这里可以添加一些游戏引擎初始化的逻辑，比如注册一些全局的对象，或者设置一些全局的参数
}
