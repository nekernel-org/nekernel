/* -------------------------------------------

	Copyright SoftwareLabs

	Purpose: Kernel Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>
#include <stdarg.h>

DK_EXTERN __attribute__((naked)) void __kernelDispatchCall(int32_t cnt, ...);

/// @brief Execute a function on the kernel.
/// @param kernelRpcName the name of the function.
/// @param cnt number of arguments.
/// @param  
/// @return 
DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, ...)
{
	if (!kernelRpcName || cnt == 0)
		return NIL;

	va_list arg;
	va_start(arg, cnt);

	__kernelDispatchCall(cnt, arg);

	va_end(arg);
}
