/* -------------------------------------------

	Copyright Theater Quality Corp.

------------------------------------------- */

#pragma once

/// @file MathMgr.h
/// @brief Linear interpolation implementation.

namespace UI
{
#ifdef ZKA_GFX_MGR_USE_DOUBLE
	typedef double fb_real_t;
#else
	typedef float fb_real_t;
#endif

	/// @brief Linear interpolation equation solver.
	/// @param from where?
	/// @param to to?
	/// @param at which state we're at **to**.
	inline fb_real_t fb_math_lerp(fb_real_t to, fb_real_t from, fb_real_t stat)
	{
		fb_real_t difference = to - from;
		return from + (difference * stat);
	}
} // namespace UI