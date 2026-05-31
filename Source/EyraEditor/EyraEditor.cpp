// Copyright Enhoney. All Rights Reserved

#include "EyraEditor.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "EyraEditor"

DEFINE_LOG_CATEGORY(LogEyraEditor);

/**
* FEyraEditorModule
*/
class FEyraEditorModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}
	
	virtual void ShutdownModule() override
	{
	}
	
};

IMPLEMENT_MODULE(FEyraEditorModule, EyraEditor);


#undef LOCTEXT_NAMESPACE
