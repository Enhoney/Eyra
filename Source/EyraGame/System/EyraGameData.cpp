// Copyright Enhoney. All Rights Reserved

#include "EyraGameData.h"

#include "EyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraGameData)

UEyraGameData::UEyraGameData()
{
}

const UEyraGameData& UEyraGameData::Get()
{
	return UEyraAssetManager::Get().GetGameData();
}
