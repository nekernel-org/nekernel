/* -------------------------------------------

    Copyright Zeta Electronics Corporation

    Purpose: Driver C++ Definitions.

------------------------------------------- */

#include <DDK/KernelStd.h>

void* operator new(size_t sz) {
    if (!sz) ++sz;

    auto ptr = kernelCall("NewKernelHeap", 1, &sz, sizeof(size_t));
    kernelCall("ProtectKernelHeap", 1, ptr, sz);

    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (!ptr) return;

    kernelCall("DeleteKernelHeap", 1, ptr, 0);
}
