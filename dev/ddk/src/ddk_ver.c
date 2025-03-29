/* -------------------------------------------

	Copyright Amlal EL Mahrouss.

	Purpose: DDK version symbols.

------------------------------------------- */

#include <DDKKit/ddk.h>

#ifndef kDDKVersionHighest
#define kDDKVersionHighest 1
#endif // !kDDKVersionHighest

#ifndef kDDKVersionLowest
#define kDDKVersionLowest 1
#endif // !kDDKVersionLowest

#ifndef kDDKVersion
#define kDDKVersion 1
#endif // !kDDKVersion

int32_t kApiVersionHighest = kDDKVersionHighest;
int32_t kApiVersionLowest  = kDDKVersionLowest;
int32_t kApiVersion		   = kDDKVersion;
