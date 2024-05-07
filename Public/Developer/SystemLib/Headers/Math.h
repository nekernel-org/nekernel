/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

///////////////////////////////////////////////////////////////////////
/// Random functions                                                ///
///////////////////////////////////////////////////////////////////////

/// @brief Number generator helper.
/// @return Random generated number.
CA_EXTERN_C SizeType MathRand(VoidType);

///////////////////////////////////////////////////////////////////////
/// Mathematical functions                                          ///
///////////////////////////////////////////////////////////////////////

CA_EXTERN_C FloatType Sqrt(FloatType number);

CA_EXTERN_C FloatType Cosine(FloatType number);
CA_EXTERN_C FloatType Sine(FloatType number);
CA_EXTERN_C FloatType Tangent(FloatType number);