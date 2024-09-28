/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <crt/defines.hxx>

/// @file Math.hxx
/// @brief Math functions.

#ifdef __ZKA_USE_DOUBLE__
typedef double real_type;
#else
typedef float real_type;
#endif

namespace std::math
{
	/// @brief Power function, with Repeat argument.
	template <size_t Exponent>
	inline real_type pow(real_type in)
	{
		if (Exponent == 0)
			return 1; // Any number to the power of 0 is 1.

		if (Exponent == 1)
			return in; // Any number to the power of 1 is itself.

		size_t cnt = Exponent;

		real_type result = 1;

		for (auto i = 0; i < cnt; ++i)
			result *= in;

		return result;
	}

	/// @brief Square of function, with Base template argument.
	/// @param of Base argument to find sqquare of
	template <size_t Base>
	inline real_type sqr(real_type in)
	{
		if (in == 0)
			return 0;

		return pow<1 / Base>(in);
	}

	/// @brief Linear interpolation equation solver.
	/// @param from where?
	/// @param to to?
	/// @param Updated diff value according to difference.
	inline real_type lerp(real_type to, real_type from, real_type stat)
	{
		real_type diff = (to - from);
		return from + (diff * stat);
	}
} // namespace std::math
