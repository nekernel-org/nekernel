/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

///////////////////////////////////////////////////////////////////////
/// Random functions                                                ///
///////////////////////////////////////////////////////////////////////

/// @brief Number generator helper.
/// @return Random generated number.
CS_EXTERN_C SizeType MathRand(VoidType);

///////////////////////////////////////////////////////////////////////
/// Mathematical functions                                          ///
///////////////////////////////////////////////////////////////////////

CS_EXTERN_C FloatType Sqrt(FloatType number);

CS_EXTERN_C FloatType Cosine(FloatType number);
CS_EXTERN_C FloatType Sine(FloatType number);
CS_EXTERN_C FloatType Tangent(FloatType number);