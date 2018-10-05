#include "Item.h"
#include "Engine/ObjectLibrary.h"
#include "UnrealType.h"

#define GRAB_SETTINGS const UItemDataHelpers* Settings = GetDefault<UItemDataHelpers>()

#if WITH_EDITOR
void UItemDataHelpers::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.MemberProperty && PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UItemDataHelpers, SearchPaths))
	{
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
		{
			// since there isn't a mechanism with which to get the specific index changed, sort them all out.
			for (FDirectoryPath& Dir : SearchPaths)
			{
				FString& String = Dir.Path;
				int32 FoundContentAt = String.Find(TEXT("/Content/"));

				if (FoundContentAt != INDEX_NONE)
				{
					String = String.RightChop(FoundContentAt);
					/** @todo(devlinw): First occurrence only */
					String.ReplaceInline(TEXT("/Content/"), TEXT("/Game/"));
				}
				else
				{
					String = TEXT("[/content/ only. delete this entry or correct it.]");
				}
			}
		}
	}
}
#endif // WITH_EDITOR
UObjectLibrary* UItemDataHelpers::InternalObjectLib = nullptr;

UObjectLibrary* UItemDataHelpers::GetItemObjectLibrary()
{
	if (!InternalObjectLib)
	{
		GRAB_SETTINGS;
		InternalObjectLib = UObjectLibrary::CreateLibrary(UItemInfo::StaticClass(), true, Settings->bUseWeakReferences);

		InternalObjectLib->AddToRoot();
	}

	// won't do anything if it thinks it has been fully loaded
	UpdateObjectLibrary();
	return InternalObjectLib;
}

void UItemDataHelpers::UpdateObjectLibrary()
{
	GRAB_SETTINGS;
	TArray<FString> RawPaths;
	for (const FDirectoryPath& SearchPath : Settings->SearchPaths)
	{
		RawPaths.Add(SearchPath.Path);
	}

	if (Settings->bFullyLoadObjectsAtStart)
	{
		InternalObjectLib->LoadBlueprintsFromPaths(RawPaths);
	}
	else
	{
		InternalObjectLib->LoadBlueprintAssetDataFromPaths(RawPaths, Settings->bForceSynchronousScan);
	}
}

void UItemDataHelpers::FullyLoadObjectLibrary()
{
	if (InternalObjectLib && !InternalObjectLib->IsLibraryFullyLoaded())
	{
		GRAB_SETTINGS;
		if (ensureMsgf(!Settings->bFullyLoadObjectsAtStart, TEXT("No need to expressly call FullyLoadObjectLibrary if you did so at start.")))
		{
			InternalObjectLib->LoadAssetsFromAssetData();
		}
		else
		{
			TArray<FString> RawPaths;

			for (const FDirectoryPath& SearchPath : Settings->SearchPaths)
			{
				RawPaths.Add(SearchPath.Path);
			}

			InternalObjectLib->LoadBlueprintsFromPaths(RawPaths);
		}
	}
}

void UItemDataHelpers::InvalidateObjectLibrary()
{
	if (InternalObjectLib)
	{
		InternalObjectLib->ClearLoaded();
		InternalObjectLib->RemoveFromRoot();
	}

	InternalObjectLib = nullptr;
}

void UItemDataHelpers::EmptyObjectLibrary()
{
	if (InternalObjectLib)
	{
		InternalObjectLib->ClearLoaded();
	}
}

UItemInfo* UItemDataHelpers::GetItemInfoByTag(const FGameplayTag& Tag, bool bExact /*= false*/)
{
	TArray<UItemInfo*> Returned = GetItemInfosByTag(Tag, bExact, 1);
	return Returned.Num() ? Returned[0] : nullptr;
}

/** @todo(devlinw): refactor */
TArray<UItemInfo*> UItemDataHelpers::GetItemInfosByTag(const FGameplayTag& Tag, bool bExact /*= false*/, int32 Max /*= -1*/)
{
	if (UObjectLibrary* Lib = GetItemObjectLibrary())
	{
		const auto DoesMatch = [bExact, Tag](const FGameplayTagContainer& Container)
		{
			return bExact ? Container.HasTagExact(Tag) : Container.HasTag(Tag);
		};

		const auto AlreadyLoaded = [Max, Lib, DoesMatch]() -> TArray<UItemInfo*>
		{
			TArray<UItemInfo*> ToRet;
			TArray<UClass*> Classes;
			// even though we may have chosen weak objects, this gives us direct pointers.
			Lib->GetObjects(Classes);
			for (UClass* AsItemInfoClass : Classes)
			{
				auto* AsItemInfo = Cast<UItemInfo>(AsItemInfoClass->GetDefaultObject());
				if (AsItemInfo && DoesMatch(AsItemInfo->ItemTags))
				{
					ToRet.Add(AsItemInfo);
					if (ToRet.Num() >= Max && Max != -1)
					{
						return ToRet;
					}
				}
			}
			return ToRet;
		};

		const auto FromAssetData = [Max, Lib, DoesMatch]() -> TArray<UItemInfo*>
		{
			TArray<UItemInfo*> ToRet;
			TArray<FAssetData> DataSet;
			Lib->GetAssetDataList(DataSet);
			for (const FAssetData& Data : DataSet)
			{
				// see if we find an asset registry searchable tag.
				if (const FString* Found = Data.TagsAndValues.Find(GET_MEMBER_NAME_CHECKED(UItemInfo, ItemTags)))
				{
					FGameplayTagContainer Reconstructed;
					Reconstructed.FromExportString(*Found);

					if (DoesMatch(Reconstructed))
					{
						/**	This actually loads the package, if it weren't obvious. */
						if (UPackage* AssetPackage = Data.GetPackage())
						{
							TArray<UObject*> WithinObjs;
							GetObjectsWithOuter(AssetPackage, WithinObjs);

							/**	Packages should only really have one generated class. */
							if (UObject** BPGC = WithinObjs.FindByPredicate([](const UObject* Test) { return Test->IsA<UBlueprintGeneratedClass>(); }))
							{
								check(BPGC && *BPGC)
								ToRet.Add(Cast<UItemInfo>((*(UBlueprintGeneratedClass**)BPGC)->GetDefaultObject()));
								if (ToRet.Num() >= Max && Max != -1)
								{
									return ToRet;
								}
							}
						}
					}
				}
			}
			return ToRet;
		};

		GRAB_SETTINGS;
		return	((Settings->bFullyLoadObjectsAtStart || Lib->IsLibraryFullyLoaded()) ? AlreadyLoaded() : FromAssetData());
	}
	return TArray<UItemInfo*>();
}

UItemInfo* UItemDataHelpers::GetItemInfoByTags(const FGameplayTagContainer& Container, bool bAll /*= true*/, bool bExact /*= false*/)
{
	TArray<UItemInfo*> Returned = GetItemInfosByTags(Container, bAll, bExact, 1);
	return Returned.Num() ? Returned[0] : nullptr;
}

/** @todo(devlinw): refactor */
TArray<UItemInfo*> UItemDataHelpers::GetItemInfosByTags(const FGameplayTagContainer& Container, bool bAll /*= true*/, bool bExact /*= false*/, int32 Max /*= -1*/)
{
	if (UObjectLibrary* Lib = GetItemObjectLibrary())
	{
		const auto DoesMatch = [bAll, bExact, Container](const FGameplayTagContainer& Container)
		{
			if (bAll)
			{
				return	bExact ? Container.HasAllExact(Container) : Container.HasAll(Container);
			}
			return	bExact ? Container.HasAnyExact(Container) : Container.HasAny(Container);
		};

		const auto AlreadyLoaded = [Max, Lib, DoesMatch]() -> TArray<UItemInfo*>
		{
			TArray<UItemInfo*> ToRet;
			TArray<UClass*> Classes;
			// even though we may have chosen weak objects, this gives us direct pointers.
			Lib->GetObjects(Classes);
			for (UClass* AsItemInfoClass : Classes)
			{
				auto* AsItemInfo = Cast<UItemInfo>(AsItemInfoClass->GetDefaultObject());
				if (AsItemInfo && DoesMatch(AsItemInfo->ItemTags))
				{
					ToRet.Add(AsItemInfo);
					if (ToRet.Num() >= Max && Max != -1)
					{
						return ToRet;
					}
				}
			}
			return ToRet;
		};

		const auto FromAssetData = [Max, Lib, DoesMatch]() -> TArray<UItemInfo*>
		{
			TArray<UItemInfo*> ToRet;
			TArray<FAssetData> DataSet;
			Lib->GetAssetDataList(DataSet);
			for (const FAssetData& Data : DataSet)
			{
				// see if we find an asset registry searchable tag.
				if (const FString* Found = Data.TagsAndValues.Find(GET_MEMBER_NAME_CHECKED(UItemInfo, ItemTags)))
				{
					FGameplayTagContainer Reconstructed;
					Reconstructed.FromExportString(*Found);

					if (DoesMatch(Reconstructed))
					{
						/**	This actually loads the package, if it weren't obvious. */
						if (UPackage* AssetPackage = Data.GetPackage())
						{
							TArray<UObject*> WithinObjs;
							GetObjectsWithOuter(AssetPackage, WithinObjs);

							/**	Packages should only really have one generated class. */
							if (UObject** BPGC = WithinObjs.FindByPredicate([](const UObject* Test) { return Test->IsA<UBlueprintGeneratedClass>(); }))
							{
								check(BPGC && *BPGC)
									ToRet.Add(Cast<UItemInfo>((*(UBlueprintGeneratedClass**)BPGC)->GetDefaultObject()));
								if (ToRet.Num() >= Max && Max != -1)
								{
									return ToRet;
								}
							}
						}
					}
				}
			}
			return ToRet;
		};

		GRAB_SETTINGS;
		return	((Settings->bFullyLoadObjectsAtStart || Lib->IsLibraryFullyLoaded()) ? AlreadyLoaded() : FromAssetData());
	}
	return TArray<UItemInfo*>();
}

#undef GRAB_SETTINGS

UItemInfo::UItemInfo()
{
	UIInfo = CreateDefaultSubobject<UItemUIInfo>(TEXT("UIInfo"));
	WorldInfo = CreateDefaultSubobject<UItemWorldInfo>(TEXT("WorldInfo"));
	CapacityInfo = CreateDefaultSubobject<UItemCapacityInfo>(TEXT("CapacityInfo"));
}
