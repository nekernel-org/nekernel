/* -------------------------------------------

	Copyright ZKA Web Services Co.

	Purpose: DDK allocator.

------------------------------------------- */

#include <ddk/ddk.h>

/**
	\brief Allocates a new heap on the Kernel's side.
	\param sz the size of the heap block.
	\return the newly allocated pointer.
*/
DK_EXTERN void* KernelAlloc(size_t sz)
{
	if (!sz)
		++sz;

	void* ptr = KernelCall("MmNewKeHeap", 1, &sz, sizeof(size_t));

	return ptr;
}

/**
	\brief Frees a pointer from the heap.
	\param ptr the pointer to free.
*/
DK_EXTERN void KernelFree(void* ptr)
{
	if (!ptr)
		return;

	KernelCall("MmDeleteKeHeap", 1, ptr, 0);
}
