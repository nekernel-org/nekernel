/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <crt/base_alloc.hxx>
#include <crt/base_exception.hxx>

#define kAllocSyscallId "mov $10, %%r9\n\t"
#define kFreeSyscallId	"mov $11, %%r9\n\t"

void* operator new(size_t length)
{
	void* ptr = nullptr;

	// do syscall according to PEF convention.
	asm("mov %0, %%r8\n\t" kAllocSyscallId
		"syscall"
		: "=r"(ptr));

	if (!ptr)
		std::__throw_bad_alloc();

	return ptr;
}

void* operator new[](size_t length)
{
	ptr_type ptr = nullptr;

	// do syscall according to PEF convention.
	asm("mov %0, %%r8\n\t" kAllocSyscallId
		"syscall"
		: "=r"(ptr));

	if (!ptr)
		std::__throw_bad_alloc();

	return ptr;
}

void operator delete(void* ptr) noexcept
{
	asm("mov %0, %%r8\n\t" kFreeSyscallId
		"syscall"
		: "=r"(ptr));
}

void operator delete[](void* ptr) noexcept
{
	asm("mov %0, %%r8\n\t" kFreeSyscallId
		"syscall"
		: "=r"(ptr));
}
