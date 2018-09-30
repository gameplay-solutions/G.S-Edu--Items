// Copyright 2018 Devlin Willis (ImmutableLambda). All Rights Reserved.

#include "GSEduItems.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogGSEduItems)


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