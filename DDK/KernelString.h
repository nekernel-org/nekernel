/* -------------------------------------------

	Copyright ZKA Technologies

	Purpose: DDK Strings.

------------------------------------------- */

#pragma once

#include <DDK/KernelStd.h>

/// @brief DDK equivalent of POSIX's string.h
/// @file kernelString.h

DK_EXTERN size_t kernelStringLength(const char* in);
DK_EXTERN int	 kernelStringCopy(char* dst, const char* src, size_t len);
