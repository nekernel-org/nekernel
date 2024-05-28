/* -------------------------------------------

	Copyright SoftwareLabs

	Purpose: Kernel Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>
#include <stdarg.h>

DK_EXTERN __attribute__((naked)) void __kernelDispatchCall(int32_t cnt, ...);

DK_EXTERN void* kernelCall(const char* kernelRpcName, int32_t cnt, ...)
{
	if (!kernelRpcName || cnt == 0)
		return NIL;

	va_list arg;
	va_start(arg, cnt);

	__kernelDispatchCall(cnt, arg);

	va_end(arg);
}
