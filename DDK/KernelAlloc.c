/* -------------------------------------------

    Copyright ZKA Technologies

    Purpose: Standard library.

------------------------------------------- */

#include <DDK/KernelStd.h>

DK_EXTERN void* kernelAlloc(size_t sz)
{
    if (!sz) ++sz;

	void* ptr = kernelCall("NewHeap", 1, &sz, sizeof(size_t));
	kernelCall("ProtectHeap", 1, ptr, sz);

    return ptr;
}

DK_EXTERN void kernelFree(void* ptr)
{
    if (!ptr) return;

	kernelCall("DeleteHeap", 1, ptr, 0);
}
