/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

/// @file Lerp.hxx
/// @brief Linear interpolation implementation.

/// @brief Linear interpolation equation solver.
/// @param from where?
/// @param to to?
/// @param at which state we're at **to**.
inline int Lerp( int from , int to , float percent ) noexcept
{
    int difference = to - from;
    return from + ( difference * percent );
}
