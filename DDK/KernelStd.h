/* -------------------------------------------

	Copyright ZKA Technologies

	Purpose: Kernel Definitions.

------------------------------------------- */

#pragma once

#if defined(__cplusplus)
#define DK_EXTERN extern "C"
#define nil		  nullptr
#define DK_FINAL final
#else
#define DK_EXTERN extern
#define nil		  ((void*)0)
#define DK_FINAL
#endif // defined(__cplusplus)

#include <stdint.h>
#include <stddef.h>

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

/// @brief allocate ptr.
/// @param sz size of ptr.
/// @return the pointer allocated or **nil**.
DK_EXTERN void* kernelAlloc(size_t sz);

/// @brief allocate ptr.
/// @param pointer to free
DK_EXTERN void kernelFree(void*);
