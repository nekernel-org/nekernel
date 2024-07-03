/* -------------------------------------------

    Copyright Zeta Electronics Corporation

    Purpose: Driver C++ Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>

void* operator new(size_t sz)
{
    return kernelAlloc(sz);
}

void operator delete(void* ptr) noexcept
{
    kernelFree(ptr);
}

DK_EXTERN void* kernelAlloc(size_t sz)
{
    if (!sz) ++sz;

	auto ptr = kernelCall("NewHeap", 1, &sz, sizeof(size_t));
	kernelCall("ProtectHeap", 1, ptr, sz);

    return ptr;
}

DK_EXTERN void kernelFree(void* ptr)
{
    if (!ptr) return;

	kernelCall("DeleteHeap", 1, ptr, 0);
}
