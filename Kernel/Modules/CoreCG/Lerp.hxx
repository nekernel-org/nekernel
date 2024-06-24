/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

/// @file Lerp.hxx
/// @brief Linear interpolation implementation.

typedef float GXReal;

/// @brief Linear interpolation equation solver.
/// @param from where?
/// @param to to?
/// @param at which state we're at **to**.
inline GXReal GXLerp(GXReal to, GXReal from, GXReal stat)
{
	GXReal difference = to - from;
	return from + (difference * stat);
}
