/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <Headers/Math.h>

/// @brief Number generator helper.
/// @return Random generated number.
CA_EXTERN_C SizeType MathRand(VoidType)
{
	return kSharedApplication->Invoke(kSharedApplication, kCallRandomNumberGenerator);
}