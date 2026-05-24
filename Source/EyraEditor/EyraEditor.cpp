// Copyright Enhoney.

#include "EyraEditor.h"
#include "Modules/ModuleManager.h"

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
