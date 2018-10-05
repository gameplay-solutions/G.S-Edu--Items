#include "Types.h"
#include "Item.h"


/**
 *	Helper template only meant for use in this file: 
 *	Iterates over the members of a given object, trying to find either a pointer or array of pointers to
 *	UInlineItemInfo - which are then queried for a given field's value (by name).
 **/
 /** @todo(devlinw): refactor? Can this be improved? */
template<typename T>
bool PullPropFromMemberInlineInfo(const UObject* From, const FName& PropertyName, typename TCallTraits<T>::Reference FoundVal)
{
	if (UClass* ObjClass = From->GetClass())
	{
		TFieldIterator<UProperty> PropIter{ ObjClass };
		while (PropIter)
		{
			if (UObjectProperty* ObjRef = Cast<UObjectProperty>(*PropIter))
			{
				if (ObjRef->PropertyClass->IsChildOf(UInlineItemInfo::StaticClass()))
				{
					if (UObject* const* ObjectReferenceRef = ObjRef->ContainerPtrToValuePtr<UObject*>(From))
					{
						if (GetFieldFromObject<T>(*ObjectReferenceRef, PropertyName, FoundVal))
						{
							return true;
						}
					}
				}
			}
			else if (UArrayProperty* ArrayProp = Cast<UArrayProperty>(*PropIter))
			{
				if (UObjectProperty* ArrObjRef = Cast<UObjectProperty>(ArrayProp->Inner))
				{
					if (ArrObjRef->PropertyClass->IsChildOf(UInlineItemInfo::StaticClass()))
					{
						FScriptArrayHelper Helper{ ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(From) };

						for (int32 i = 0; i < Helper.Num(); ++i)
						{
							if (UObject** ObjectReferenceRef = (UObject**)Helper.GetRawPtr(i))
							{
								if (GetFieldFromObject<T>(*ObjectReferenceRef, PropertyName, FoundVal))
								{
									return true;
								}
							}
						}
					}
				}
			}

			++PropIter;
		}
	}
	return false;
}

bool USimpleTestsLib::GetFloatByName(const UObject* From, const FName& PropertyName, float& FoundFloat)
{
	FoundFloat = FLT_MAX;
	if (GetFieldFromObject<float>(From, PropertyName, FoundFloat))
	{
		return true;
	}
	
	if (const UItemInfo* ItemInfo = Cast<UItemInfo>(From))
	{
		if (GetFloatByName(ItemInfo->UIInfo, PropertyName, FoundFloat)
		  ||GetFloatByName(ItemInfo->WorldInfo, PropertyName, FoundFloat)
		  ||GetFloatByName(ItemInfo->CapacityInfo, PropertyName, FoundFloat))
		{
			return true;
		}
	}

	return PullPropFromMemberInlineInfo<float>(From, PropertyName, FoundFloat);
}
