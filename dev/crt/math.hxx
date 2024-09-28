/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

/// @file Math.hxx
/// @brief Math functions.

namespace CRT
{
	using namespace Kernel;

#ifdef __ZKA_USE_DOUBLE__
	typedef double Real;
#else
	typedef float Real;
#endif

	/// @brief Power function, with Repeat argument.
	template <SizeT Exponent>
	inline Real Pow(Real in)
	{
		if (Exponent == 0)
			return 1; // Any number to the power of 0 is 1.

		if (Exponent == 1)
			return in; // Any number to the power of 1 is itself.

		UInt64 cnt = Exponent;

		Real result = 1;

		for (auto i = 0; i < (cnt); ++i)
			result *= in;

		return result;
	}

	/// @brief Square of function, with Base template argument.
	/// @param of Base argument to find sqquare of
	template <SizeT Base>
	inline Real SqrOf(Real in)
	{
		if (in == 0)
			return 0;

		return Pow<1 / Base>(in);
	}

	/// @brief Linear interpolation equation solver.
	/// @param from where?
	/// @param to to?
	/// @param Updated diff value according to difference.
	inline Real CGLerp(Real to, Real from, Real stat)
	{
		Real diff = (to - from);
		return from + (diff * stat);
	}
} // namespace CRT
