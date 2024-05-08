/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: Driver C++ Definitions.

------------------------------------------- */

#include <DriverKit/KernelStd.h>

void* operator new(size_t sz) {
    if (!sz) ++sz;

    auto ptr = kernelCall("NewKernelHeap", 1, sz);
    kernelCall("KernelHeapProtect", 1, ptr);

    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (!ptr) return;

    kernelCall("DeleteKernelHeap", 1,ptr);
}
