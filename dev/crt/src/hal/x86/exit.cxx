/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <crt/exit.hxx>

#define kAllocSyscallId "mov $12, %%r9\n\t"

/// @note Just here for building.
extern "C" int exit(int code)
{
	asm("mov 0, %%r8\n\t" kAllocSyscallId
		"syscall"
		: "=r"(code));

	return 1;
}
