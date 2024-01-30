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

	return HCore::kernel_new_ptr(sz, true, false); 
}

void* operator new(size_t sz) 
{
	if (sz == 0)
		++sz;

	return HCore::kernel_new_ptr(sz, true, false); 
}

void operator delete[](void* ptr) 
{
    if (ptr == nullptr)
        return;

    HCore::kernel_delete_ptr(ptr); 
}

void operator delete(void* ptr)
{
    if (ptr == nullptr)
        return;

    HCore::kernel_delete_ptr(ptr); 
}

void operator delete(void* ptr, size_t sz)
{
    if (ptr == nullptr)
        return;

    (void)sz;

    HCore::kernel_delete_ptr(ptr); 
}

