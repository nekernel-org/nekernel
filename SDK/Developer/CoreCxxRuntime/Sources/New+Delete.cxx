/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <Headers/Heap.h>

typedef SizeType size_t;

void* operator new[](size_t sz)
{
	if (sz == 0)
		++sz;

	return RtTlsAllocate(sz, kStandardAllocation);
}

void* operator new(size_t sz)
{
	if (sz == 0)
		++sz;

	return RtTlsAllocate(sz, kArrayAllocation);
}

void operator delete[](void* ptr)
{
	if (ptr == nullptr)
		return;

	RtTlsFree(ptr);
}