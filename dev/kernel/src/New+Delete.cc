/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/HeapMgr.h>
#include <NeKit/New.h>

void* operator new[](size_t sz) {
  if (sz == 0) ++sz;

  return Kernel::mm_alloc_ptr(sz, true, false);
}

void* operator new(size_t sz) {
  if (sz == 0) ++sz;

  return Kernel::mm_alloc_ptr(sz, true, false);
}

void operator delete[](void* ptr) {
  if (ptr == nullptr) return;

  Kernel::mm_free_ptr(ptr);
}

void operator delete(void* ptr) {
  if (ptr == nullptr) return;

  Kernel::mm_free_ptr(ptr);
}

void operator delete(void* ptr, size_t sz) {
  if (ptr == nullptr) return;

  NE_UNUSED(sz);

  Kernel::mm_free_ptr(ptr);
}

void operator delete[](void* ptr, size_t sz) {
  if (ptr == nullptr) return;

  NE_UNUSED(sz);

  Kernel::mm_free_ptr(ptr);
}
