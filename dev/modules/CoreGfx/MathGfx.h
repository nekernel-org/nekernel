/* -------------------------------------------

	Copyright Amlal El Mahrouss.

------------------------------------------- */

#pragma once

/// @file MathMgr.h
/// @brief Linear interpolation implementation.

namespace UI
{
#ifdef NE_GFX_MGR_USE_DOUBLE
	typedef double fb_real_t;
#else
	typedef float fb_real_t;
#endif

	/// @brief Linear interpolation equation solver.
	/// @param from where to start
	/// @param to to which value.
	/// @param stat
	/// @return Linear interop value.
	inline fb_real_t fb_math_lerp(fb_real_t to, fb_real_t from, fb_real_t stat)
	{
		return (from) + (to - from) * stat;
	}
} // namespace UI