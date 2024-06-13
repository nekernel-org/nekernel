/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	Purpose: Kernel Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>
#include <stdarg.h>

/// @brief this is an internal call, do not use it.
DK_EXTERN __attribute__((naked)) void* __kernelDispatchCall(const char* name, int32_t cnt, void* data, size_t sz);

/// @brief Execute a function on the kernel.
/// @param kernelRpcName the name of the function.
/// @param cnt number of arguments.
/// @param
/// @return
DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, void* data, size_t sz)
{
	if (!kernelRpcName || cnt == 0)
		return NIL;

	return __kernelDispatchCall(kernelRpcName, cnt, data, sz);
}
