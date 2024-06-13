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

/// @brief Call kernel (interrupt 0x33)
/// @param kernelRpcName 
/// @param cnt 
/// @param dat 
/// @param sz 
/// @return 
DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, void* dat, size_t sz);
