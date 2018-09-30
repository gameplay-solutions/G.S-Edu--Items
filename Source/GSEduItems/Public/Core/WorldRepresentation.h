#pragma once

#include "Interface.h"
#include "WorldRepresentation.generated.h"

class UItemInfo;
class AController;

/**
 *	@todo(devlinw): Fucking fix this or work around it.
 *	Great fucking choice here, epic.
 **/
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UItemWorldRepresentation : public UInterface
{ GENERATED_BODY() };

class IItemWorldRepresentation
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	virtual UItemInfo* GetRepresentedItem() const { return nullptr; }

	UFUNCTION(BlueprintCallable)
	virtual bool CanBePickedUp() const { return false; }
	
	UFUNCTION(BlueprintCallable)
	virtual bool PickedUpBy(AController* Entity) { if (CanBePickedUp()) RemovedFromWorld(); return CanBePickedUp(); }

	UFUNCTION(BlueprintCallable)
	virtual void RemovedFromWorld(){}

	UFUNCTION(BlueprintCallable)
	virtual void IntroducedIntoWorld(){}

	UFUNCTION(BlueprintCallable)
	virtual void DroppedBy(AController* Entity) { IntroducedIntoWorld();}
};