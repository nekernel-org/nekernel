/* -------------------------------------------

	Copyright ZKA Technologies.

	Purpose: DDK kernel call.

------------------------------------------- */

#include <DDK/KernelStd.h>
#include <stdarg.h>

/// @brief this is an internal call, do not use it.
DK_EXTERN __attribute__((naked)) void* __kernelCallDispatch(const char* name, int32_t cnt, void* data, size_t sz);

/// @brief Interupt kernel
/// @param kernelRpcName RPC name
/// @param cnt number of elements in **data** pointer.
/// @param data data pointer.
/// @param sz The size of the whole data pointer.
/// @retval void* kernel call was successful.
/// @retval nil kernel call failed, call kernelLastError(void)
DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, void* data, size_t sz)
{
	if (!kernelRpcName || cnt == 0)
		return nil;

	return __kernelCallDispatch(kernelRpcName, cnt, data, sz);
}

/// @brief Add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DK_EXTERN void kernelAddSyscall(const int slot, void (*slotFn)(void* a0))
{
	kernelCall("AddSyscall", slot, slotFn, 1);
}

/// @brief Get a kernel property.
/// @param slot property id (always 0)
/// @param name the prperty's name.
/// @return property's object.
DK_EXTERN void* kernelGetProperty(const int slot, const char* name)
{
	return kernelCall("GetProperty", slot, name, 1);
}

/// @brief Set a kernel property.
/// @param slot property id (always 0)
/// @param name the property's name.
/// @param ddk_pr pointer to a  property's DDK_PROPERTY_RECORD.
/// @return property's object.
DK_EXTERN void* kernelSetProperty(const int slot, const struct DDK_PROPERTY_RECORD* ddk_pr)
{
	return kernelCall("SetProperty", slot, ddk_pr, 1);
}
