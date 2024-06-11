/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <Headers/Math.h>

/// @brief Number generator helper.
/// @return Random generated number.
CS_EXTERN_C SizeType MathRand(VoidType)
{
	return kSharedApplication->Invoke(kSharedApplication, kCallRandomNumberGenerator);
}