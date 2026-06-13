// Copyright Enhoney. All Rights Reserved

#include "EyraEditor.h"
#include "Modules/ModuleManager.h"
#include "GameModes/EyraExperienceManager.h"

#define LOCTEXT_NAMESPACE "EyraEditor"

DEFINE_LOG_CATEGORY(LogEyraEditor);

/**
* FEyraEditorModule
*/
class FEyraEditorModule : public FDefaultGameModuleImpl
{
	//必须要在这里进行定义 否则用不了ThisClass 因为这个类没有反射，没有定义ThisClass
	typedef FEyraEditorModule ThisClass;

	virtual void StartupModule() override
	{
		if (!IsRunningGame())
		{


			FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
			FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
		}
	}
	
	virtual void ShutdownModule() override
	{
	}

	void OnBeginPIE(bool bIsSimulating)
	{
		UEyraExperienceManager* ExperienceManager = GEngine->GetEngineSubsystem<UEyraExperienceManager>();
		check(ExperienceManager);
		ExperienceManager->OnPlayInEditorBegun();
	}

	void OnEndPIE(bool bIsSimulating)
	{

	}
	
};

IMPLEMENT_MODULE(FEyraEditorModule, EyraEditor);


#undef LOCTEXT_NAMESPACE
