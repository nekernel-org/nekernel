
/* -------------------------------------------

	Copyright (C) 2024, t& Labs, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/Heap.h>

/// @note compatible with tk too.
typedef __SIZE_TYPE__ size_t;

void* operator new(size_t ptr);
void* operator new[](size_t ptr);

void operator delete(void* ptr);
void operator delete(void* ptr, unsigned long);
void operator delete[](void* ptr);
