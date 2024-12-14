/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/Heap.h>
#include <NewKit/New.h>

void* operator new[](size_t sz)
{
	if (sz == 0)
		++sz;

	return Kernel::mm_new_heap(sz, true, false);
}

void* operator new(size_t sz)
{
	if (sz == 0)
		++sz;

	return Kernel::mm_new_heap(sz, true, false);
}

void operator delete[](void* ptr)
{
	if (ptr == nullptr)
		return;

	Kernel::mm_delete_heap(ptr);
}

void operator delete(void* ptr)
{
	if (ptr == nullptr)
		return;

	Kernel::mm_delete_heap(ptr);
}

void operator delete(void* ptr, size_t sz)
{
	if (ptr == nullptr)
		return;

	ZKA_UNUSED(sz);

	Kernel::mm_delete_heap(ptr);
}
