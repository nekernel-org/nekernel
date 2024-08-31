/* -------------------------------------------

	Copyright ZKA Technologies.

	Purpose: DDK Kernel call.

------------------------------------------- */

#include <DDK/KernelStd.h>
#include <stdarg.h>

/// @brief this is an internal call, do not use it.
DK_EXTERN __attribute__((naked)) void* __KernelCallDispatch(const char* name, int32_t cnt, void* data, size_t sz);

/// @brief Interupt Kernel
/// @param KernelRpcName RPC name
/// @param cnt number of elements in **data** pointer.
/// @param data data pointer.
/// @param sz The size of the whole data pointer.
/// @retval void* Kernel call was successful.
/// @retval nil Kernel call failed, call KernelLastError(void)
DK_EXTERN void* KernelCall(const char* KernelRpcName, int32_t cnt, void* data, size_t sz)
{
	if (!KernelRpcName || cnt == 0)
		return nil;

	return __KernelCallDispatch(KernelRpcName, cnt, data, sz);
}

/// @brief Add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DK_EXTERN void KernelAddSyscall(const int slot, void (*slotFn)(void* a0))
{
	KernelCall("AddSyscall", slot, slotFn, 1);
}

/// @brief Get a Kernel property.
/// @param slot property id (always 0)
/// @param name the prperty's name.
/// @return property's object.
DK_EXTERN void* KernelGetProperty(const int slot, const char* name)
{
	return KernelCall("GetProperty", slot, name, 1);
}

/// @brief Set a Kernel property.
/// @param slot property id (always 0)
/// @param name the property's name.
/// @param ddk_pr pointer to a  property's DDK_PROPERTY_RECORD.
/// @return property's object.
DK_EXTERN void* KernelSetProperty(const int slot, const struct DDK_PROPERTY_RECORD* ddk_pr)
{
	return KernelCall("SetProperty", slot, ddk_pr, 1);
}
