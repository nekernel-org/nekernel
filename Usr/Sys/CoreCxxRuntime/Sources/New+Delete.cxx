/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <Headers/Heap.h>

typedef SizeType size_t;

void* operator new[](size_t sz)
{
	if (sz == 0)
		++sz;

	return RtHeapAllocate(sz, kStandardAllocation);
}

void* operator new(size_t sz)
{
	if (sz == 0)
		++sz;

	return RtHeapAllocate(sz, kArrayAllocation);
}

void operator delete[](void* ptr)
{
	if (ptr == nullptr)
		return;

	RtHeapFree(ptr);
}