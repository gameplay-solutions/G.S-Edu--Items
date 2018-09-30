#include "Types.h"

bool USimpleTestsLib::GetFloatFromInfoStruct(const FItemUIInfo& Info, const FName& PropertyName, float& FoundFloat)
{
	FoundFloat = FLT_MAX;
	for (const UInlineItemInfo* Misc : Info.MiscInfo)
	{
		if (GetFieldFromObject<float>(Misc, PropertyName, FoundFloat))
		{
			return true;
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
	return false;
}
