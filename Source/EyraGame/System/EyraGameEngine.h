// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Engine/GameEngine.h"

#include "EyraGameEngine.generated.h"

class IEngineLoop;
class UObject;


UCLASS()
class UEyraGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UEyraGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};

