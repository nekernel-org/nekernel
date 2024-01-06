
/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */
#pragma once

#include <NewKit/KHeap.hpp>

typedef __SIZE_TYPE__ size_t; // gcc will complain about that

void* operator new(size_t ptr);
void* operator new[](size_t ptr);

void operator delete(void* ptr);
void operator delete(void* ptr, unsigned long);
void operator delete[](void* ptr);
