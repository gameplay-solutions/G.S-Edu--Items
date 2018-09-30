#pragma once

#include "AssertionMacros.h"
#include "Class.h"
#include "IsEnum.h"
#include "ObjectMacros.h"
#include "UObjectGlobals.h"
#include "UnrealString.h"

/**
*	Defines two functions based on the type you pass in. If it isn't a type, it won't compile.
*	If the type passed in isn't an enum type, you'll get a compile error.
*
*	You should only use this enum within the namespace EnumHelpers.
*
*	Type_Reflected is a function that returns a UEnum for your type.
*	ToString is a template function which is specialized for each enum type. It returns the string name for a given value.
**/
#define ENUM_ACCESSORS(Type) \
UEnum* Type##_Reflected ()  \
{ \
    static const UEnum* Found = nullptr; \
    if (!Found || Found->HasAnyFlags(RF_NewerVersionExists)) Found = FindObject<UEnum>(ANY_PACKAGE, TEXT(#Type), true); \
    return Found; \
} \
template<> \
FString ToString<Type>(Type Value) \
{ \
	static_assert(TIsEnum<Type>::Value, "Only use ENUM_ACCESSORS with enum types."); \
	if (UEnum* Found = Type##_Reflected()) \
	{ \
		return Found->GetNameStringByValue(Value); \
	} \
	return TEXT(""); \
}

#define LAZY_ENUM_ACCESSORS(Type)  \
namespace EnumHelpers \
{ \
	ENUM_ACCESSORS(Type) \
}

/**
*	EnumHelpers will contain all ToString and _Reflected functions for enum types unless
*	you do not define them within this namespace.
**/
namespace EnumHelpers
{
	template<enum T>
	ToString(T Value)
	{
		static_assert(TIsEnum<T>::Value, "Should only call EnumHelpers::ToString with enum types");
		ensureMsgf(false, TEXT("Error: Please specify an ENUM_ACCESSORS for the presently unknown type."));
		return TEXT("");
	}
}

/**
*	This macro is more expensive, and you should really never use it. I'm mostly just curious about how it'd work.
**/
#define AS_STRING(Type, Val) \
[=]() \
{ \
	if (const UEnum* Found = FindObject<UEnum>(ANY_PACKAGE, TEXT(#Type), true)) \
	{ \
		return Found->GetNameStringByValue(Val); \
	} \
	return FString(TEXT("")); \
}()