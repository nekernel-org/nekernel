/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK version symbols.

------------------------------------------- */

#include <DDKKit/ddk.h>

#ifndef kDDKVersionHighest
#define kDDKVersionHighest 1
#endif  // !kDDKVersionHighest

#ifndef kDDKVersionLowest
#define kDDKVersionLowest 1
#endif  // !kDDKVersionLowest

#ifndef kDDKVersion
#define kDDKVersion 1
#endif  // !kDDKVersion

uint32_t kApiVersionHighest = kDDKVersionHighest;
uint32_t kApiVersionLowest  = kDDKVersionLowest;
uint32_t kApiVersion        = kDDKVersion;
