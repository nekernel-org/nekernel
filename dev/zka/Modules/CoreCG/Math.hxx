/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

/// @file Math.hxx
/// @brief Linear interpolation implementation.

typedef float CGReal;

/// @brief Linear interpolation equation solver.
/// @param from where?
/// @param to to?
/// @param at which state we're at **to**.
inline CGReal CGLerp(CGReal to, CGReal from, CGReal stat)
{
	CGReal difference = to - from;
	return from + (difference * stat);
}
