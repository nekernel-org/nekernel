/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK Strings.

------------------------------------------- */

#pragma once

#include <DDKKit/ddk.h>

/// @brief DDK equivalent of POSIX's string.h
/// @file str.h

DDK_EXTERN size_t kstrlen(const char* in);
DDK_EXTERN int    kstrncpy(char* dst, const char* src, size_t len);
