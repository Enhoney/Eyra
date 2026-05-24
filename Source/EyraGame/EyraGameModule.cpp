// Copyright Enhoney. All Rights Reserved

#include "Modules/ModuleManager.h"

/***
* FEyraGameModule
*
*/

class FEyraGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}
	
	virtual void ShutdownModule() override
	{
	}
		
};

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, EyraGame, "EyraGame" );
