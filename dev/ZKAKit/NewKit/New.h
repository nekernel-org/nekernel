
/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <KernelKit/Heap.h>

typedef __SIZE_TYPE__ size_t; // gcc will complain about that

void* operator new(size_t ptr);
void* operator new[](size_t ptr);

void operator delete(void* ptr);
void operator delete(void* ptr, unsigned long);
void operator delete[](void* ptr);
