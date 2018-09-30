#pragma once

#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGSEduItems, Log, All)
/**	Convenience wrapper to UE_LOG, specifying the module's category by default. */
#define MOD_LOG(Verbosity, Format, ...) UE_LOG(LogGSEduItems, Verbosity, Format, __VA_ARGS__)