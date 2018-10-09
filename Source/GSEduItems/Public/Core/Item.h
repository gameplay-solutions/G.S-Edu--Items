#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameplayTagContainer.h"
#include "Types.h"
#include "Engine/EngineTypes.h"
#include "WorldRepresentation.h"
#include "Item.generated.h"

class UObjectLibrary;

UCLASS(Blueprintable, EarlyAccessPreview)
class GSEDUITEMS_API UItemInfo : public UObject
{
	GENERATED_BODY()
public:
	
	UItemInfo();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable)
	FGameplayTagContainer ItemTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UItemWorldInfo*	WorldInfo;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UItemUIInfo* UIInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UItemCapacityInfo* CapacityInfo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSubclassOf<UObject> ActualItemClass;

	template <typename T>
	static const T* GetInfoFor(UObject* Object)
	{
		if (Object)
		{
			return GetInfoFor(Object->GetClass());
		}

		return nullptr;
	}

	template <typename T>
	static const T* GetInfoFor(TSubclassOf<UObject> Class)
	{
		if (auto* Found = ItemToInfoMap.Find(Class))
		{
			return Cast<T>(*Found->GetDefaultObject());
		}

		return nullptr;
	}

	template <typename T>
	static TSubclassOf<T> GetItemClassFrom(UItemInfo* Info)
	{
		if (Info)
		{
			return GetItemClassFrom(Info->GetClass());
		}

		return nullptr;
	}

	
	template <typename T>
	static TSubclassOf<T> GetItemClassFrom(TSubclassOf<UItemInfo> Class)
	{
		if (auto* Found = InfoToItemMap.Find(Class))
		{
			return TSubclassOf<T>(*Found);
		}
		
		return nullptr;
	}

private:

	virtual	void PreEditChange(UProperty* PropertyAboutToChange ) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	static TMap<TSubclassOf<UItemInfo>, TSubclassOf<UObject>> InfoToItemMap;
	static TMap<TSubclassOf<UObject>, TSubclassOf<UItemInfo>> ItemToInfoMap;
};

UCLASS(Config=Items)
class GSEDUITEMS_API UItemDataHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	/**	BEGIN: Settings stuff which is non-static */
	
	/**	Whether to load objects directly, or use the asset registry to defer loading. */
	UPROPERTY(EditDefaultsOnly, Config)
	bool bFullyLoadObjectsAtStart = false;
	
	/**	Whether to use weak references to iteminfo classes. (only really good in the editor) */
	UPROPERTY(EditDefaultsOnly, Config)
	bool bUseWeakReferences = false;

	/**	Whether a scan should be conducted before attempting to load asset data from the asset registry. (Editor Only).  */
	UPROPERTY(EditDefaultsOnly, Config)
	bool bForceSynchronousScan = false;

	/**	The paths to search within. */
	UPROPERTY(EditDefaultsOnly, Config)
	TArray<FDirectoryPath> SearchPaths;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	/**	END: Settings stuff which is non-static */

	static UObjectLibrary* GetItemObjectLibrary();

	/**	Update the object library, unless its been fully loaded. */
	UFUNCTION(BlueprintCallable, Category = "GSEduItems|ItemData|Advanced")
	static void UpdateObjectLibrary();

	/**	Fully load the object library. */
	UFUNCTION(BlueprintCallable, Category = "GSEduItems|ItemData|Advanced")
	static void FullyLoadObjectLibrary();

	/**	Will clear the object library, and no longer prevent it from being GCd. */
	UFUNCTION(BlueprintCallable, Category = "GSEduItems|ItemData|Advanced")
	static void InvalidateObjectLibrary();

	/**	Clear the object library, but retain it. */
	UFUNCTION(BlueprintCallable, Category = "GSEduItems|ItemData|Advanced")
	static void EmptyObjectLibrary();

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Tag"), Category = "GSEduItems|ItemData|Accessors")
	static UItemInfo* GetItemInfoByTag(const FGameplayTag& Tag, bool bExact = false);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Tag", AdvancedDisplay="Max"), Category = "GSEduItems|ItemData|Accessors")
	static TArray<UItemInfo*> GetItemInfosByTag(const FGameplayTag& Tag, bool bExact = false, int32 Max = -1);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Container"), Category = "GSEduItems|ItemData|Accessors")
	static UItemInfo* GetItemInfoByTags(const FGameplayTagContainer& Container, bool bAll = true, bool bExact = false);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Container", AdvancedDisplay = "Max"), Category = "GSEduItems|ItemData|Accessors")
	static TArray<UItemInfo*> GetItemInfosByTags(const FGameplayTagContainer& Container, bool bAll = true, bool bExact = false, int32 Max = -1);
																										
private:
	static UObjectLibrary* InternalObjectLib;
};


UCLASS(BlueprintType, Blueprintable)
class GSEDUITEMS_API ADefaultItemActor : public AActor, public IItemWorldRepresentation
{
	GENERATED_BODY()
};