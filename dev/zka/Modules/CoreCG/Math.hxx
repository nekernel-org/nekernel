/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

/// @file Math.hxx
/// @brief Math functions.

namespace CG
{
	using namespace Kernel;

#ifdef __ZKA_USE_DOUBLE__
	typedef double CGReal;
#else
	typedef float CGReal;
#endif

	/// @brief Square of function, with Base template argument.
	/// @param of Base argument to find sqquare of
	template <CGReal Base>
	inline CGReal SqrOf(CGReal of)
	{
		return (of / (1.0 / Base));
	}

	/// @brief Power function, with Repeat argument.
	template <CGReal Exponent>
	inline CGReal Pow(CGReal in)
	{
		if (Exponent == 0)
			return 1; // Any number to the power of 0 is 1.

		if (Exponent == 1)
			return in; // Any number to the power of 1 is itself.

		Int32 cnt = Exponent;

		CGReal result = 1;

		for (auto i = 0; i < (cnt); ++i)
			result *= in;

		return result;
	}

	/// @brief Linear interpolation equation solver.
	/// @param from where?
	/// @param to to?
	/// @param at which state we're at **to**.
	inline CGReal CGLerp(CGReal to, CGReal from, CGReal stat)
	{
		CGReal diff = (to - from);
		return from + (diff * stat);
	}
} // namespace CG
