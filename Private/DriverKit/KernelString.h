/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: Kernel Strings.

------------------------------------------- */

#pragma once

#include <DriverKit/KernelStd.h>

/// @brief DriverKit equivalent of POSIX's string.h.

DK_EXTERN size_t kernelStringLength(const char* str);
DK_EXTERN int kernelStringCopy(char* dst, const char* src, size_t len);
