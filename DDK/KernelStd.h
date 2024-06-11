/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Definitions.

------------------------------------------- */

#pragma once

#if defined(__cplusplus)
#define DK_EXTERN extern "C"
#define NIL		  nullptr
#else
#define DK_EXTERN extern
#define NIL		  NULL
#endif // defined(__cplusplus)

#include <stdint.h>
#include <stddef.h>

DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, ...);
