// Copyright Enhoney. All Rights Reserved

#include "EyraExperienceManagerComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraExperienceManagerComponent)

UEyraExperienceManagerComponent::UEyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UEyraExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UEyraExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	return false;
}
