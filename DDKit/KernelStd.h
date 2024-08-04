/* -------------------------------------------

	Copyright ZKA Technologies

	Purpose: DDK Definitions.

------------------------------------------- */

#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
#define DK_EXTERN extern "C"
#define nil		  nullptr
#define DK_FINAL final
#else
#define DK_EXTERN extern
#define nil		  ((void*)0)
#define DK_FINAL
#endif // defined(__cplusplus)

#ifndef __NEWOSKRNL__
#error !!! including header in kernel mode !!!
#endif // __NEWOSKRNL__

struct DDK_STATUS_STRUCT;

/// \brief DDK status structure (__at_enable, __at_disable...)
struct DDK_STATUS_STRUCT DK_FINAL
{
	int32_t action_id;
	int32_t issuer_id;
	int32_t group_id;
};

/// @brief Call kernel (interrupt 0x33)
/// @param kernelRpcName
/// @param cnt number of elements in **dat**
/// @param dat data ptr
/// @param sz sz of whole data ptr.
/// @return result of call
DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, void* dat, size_t sz);

/// @brief add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DK_EXTERN void kernelAddSyscall(const int slot, void (*slotFn)(void* a0));

/// @brief allocate heap ptr.
/// @param sz size of ptr.
/// @return the pointer allocated or **nil**.
DK_EXTERN void* kernelAlloc(size_t sz);

/// @brief free heap ptr.
/// @param pointer to free
DK_EXTERN void kernelFree(void*);

/// @brief The highest API version of the DDK.
DK_EXTERN int32_t c_api_version_highest;

/// @brief The lowest API version of the DDK.
DK_EXTERN int32_t c_api_version_least;

/// @brief c_api_version_least+c_api_version_highest combined version.
DK_EXTERN int32_t c_api_version;