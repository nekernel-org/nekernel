/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Heap.hxx>

void* operator new[](size_t sz) 
{
	if (sz == 0)
		++sz;

	return HcAllocateProcessHeap(kInstanceObject, sz, kStandardAllocation); 
}

void* operator new(size_t sz) 
{
	if (sz == 0)
		++sz;

	return HcAllocateProcessHeap(kInstanceObject, sz, kArrayAllocation);
}

void operator delete[](void* ptr) 
{
    if (ptr == nullptr)
        return;

    HcFreeProcessHeap(kInstanceObject, ptr);
}

void operator delete(void* ptr)
{
    if (ptr == nullptr)
        return;

    HcFreeProcessHeap(kInstanceObject, ptr);
}

void operator delete(void* ptr, size_t sz)
{
    if (ptr == nullptr)
        return;

    (void)sz;

    HcFreeProcessHeap(kInstanceObject, ptr);
}

