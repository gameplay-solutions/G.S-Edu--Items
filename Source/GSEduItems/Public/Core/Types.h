#pragma once

#include "Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealTypeTraits.h"

#include "Types.generated.h"

/**	
 *	Base class for all InlineItemInfo.
 *	
 *	While each can represent a single property, items should bundle their
 *	data as much as possible. 
 **/
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, editinlinenew, EarlyAccessPreview)
class GSEDUITEMS_API UInlineItemInfo : public UObject
{
	GENERATED_BODY()
};

/**
 *	An easier way to pull values from an object using reflection.
 *	Will make working with InlineItemInfo much easier, as they can be
 *	added through the editor.
 *	
 *	Usage: 
 *	float ReflectedFloat = FLT_MAX;
 *	if (GetFieldFromObject<float>(SomeActor, TEXT("Health"), ReflectedFloat))
 *	{
 *		// process health we pulled from the actor.
 *	}
 **/
template<typename T>
bool GetFieldFromObject(const UObject* Object, const FName FieldName, typename TCallTraits<T>::Reference OutVal)
{
	if (const UClass* ObjClass = (Object ? Object->GetClass() : nullptr))
	{
		const UProperty* Found = ObjClass->FindPropertyByName(FieldName);
		/** @todo(devlinw): Maybe also define a TReflFromNative<T>::PropType? */
		if (Found && sizeof(T) == Found->ElementSize)
		{
			if (TIsPODType<T>::Value && Found->HasAnyPropertyFlags(CPF_IsPlainOldData))
			{
				OutVal = *Found->ContainerPtrToValuePtr<T>(Object);
			}
			else
			{
				/** @todo(devlinw): This func doesn't work for POD, maybe it doesn't work for other fields?? */
				Found->CopyCompleteValue(&OutVal, Object);
			}
			return true;
		}
	}
	return false;
}

/**
 *	The following aren't InlineItemInfo because they're provided by this module and make less sense as that construct.
 **/

// Basic User Interface Data
UCLASS(BlueprintType, DefaultToInstanced, EarlyAccessPreview, editinlinenew, Within=ItemInfo)
class GSEDUITEMS_API UItemUIInfo : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush DisplayImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UInlineItemInfo*> MiscInfo;
};

UCLASS(BlueprintType, DefaultToInstanced, EarlyAccessPreview, editinlinenew, Within = ItemInfo)
class GSEDUITEMS_API UItemWorldInfo : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "ItemWorldRepresentation", NoClear))
	TSubclassOf<AActor> ActorRepresentation;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UInlineItemInfo*> MiscInfo;
};


UCLASS(BlueprintType, DefaultToInstanced, EarlyAccessPreview, editinlinenew, Within = ItemInfo)
class GSEDUITEMS_API UItemCapacityInfo : public UObject
{
	GENERATED_BODY()
public: 
	/**	
	 *	Zero or one indicate no stacking, -1 or less indicates infinite stacking;
	 *	Any number greater than one is the maximum amount of items that can stack. 
	 **/
	int32 MaximumStack;
	float Weight;
	float MaximumDurability;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UInlineItemInfo*> MiscInfo;
};


/** @note(devlinw): This will eventually become a more powerful lib for getting values from inlineinfo or objects containing them. */
UCLASS()
class USimpleTestsLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:


	UFUNCTION(BlueprintPure, meta = (AutoCreateRefTerm = "PropertyName"))
	static bool GetFloatByName(const UObject* From, const FName& PropertyName, float& FoundFloat);
};