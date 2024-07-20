/* -------------------------------------------

    Copyright ZKA Technologies

    Purpose: Driver C++ Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>

void* operator new(size_t sz)
{
    return kernelAlloc(sz);
}

void operator delete(void* ptr)
{
    kernelFree(ptr);
}

void* operator new[](size_t sz)
{
    return kernelAlloc(sz);
}

void operator delete[](void* ptr)
{
    kernelFree(ptr);
}
