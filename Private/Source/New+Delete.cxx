/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/KernelHeap.hpp>
#include <NewKit/New.hpp>

void* operator new[](size_t sz) 
{
	if (sz == 0)
		++sz;

	return HCore::ke_new_ke_heap(sz, true, false); 
}

void* operator new(size_t sz) 
{
	if (sz == 0)
		++sz;

	return HCore::ke_new_ke_heap(sz, true, false); 
}

void operator delete[](void* ptr) 
{
    if (ptr == nullptr)
        return;

    HCore::ke_delete_ke_heap(ptr); 
}

void operator delete(void* ptr)
{
    if (ptr == nullptr)
        return;

    HCore::ke_delete_ke_heap(ptr); 
}

void operator delete(void* ptr, size_t sz)
{
    if (ptr == nullptr)
        return;

    (void)sz;

    HCore::ke_delete_ke_heap(ptr); 
}

