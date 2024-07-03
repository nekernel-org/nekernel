/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>
#include <stdarg.h>

/// @brief this is an internal call, do not use it.
DK_EXTERN __attribute__((naked)) void* __kernelDispatchCall(const char* name, int32_t cnt, void* data, size_t sz);

/// @brief Call kernel (interrupt 0x33)
/// @param kernelRpcName
/// @param cnt number of elements in **dat**
/// @param dat data ptr
/// @param sz sz of whole data ptr.
/// @return result of call
DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, void* data, size_t sz)
{
	if (!kernelRpcName || cnt == 0)
		return nil;

	return __kernelDispatchCall(kernelRpcName, cnt, data, sz);
}


/// @brief add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DK_EXTERN void  kernelAddSyscall(const int slot, void(*slotFn)(void* a0))
{
	kernelCall("AddSyscall", slot, slotFn, 0);
}