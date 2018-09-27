// Copyright 2018 Devlin Willis (ImmutableLambda). All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FGSEduItems : public FDefaultModuleImpl
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FGSEduItems, GSEduItems)

void FGSEduItems::StartupModule()
{
}

void FGSEduItems::ShutdownModule()
{
}