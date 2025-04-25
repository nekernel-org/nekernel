/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK C++ runtime.

------------------------------------------- */

#include <DDKKit/ddk.h>

void* operator new(size_t sz) {
  return kalloc(sz);
}

void operator delete(void* ptr) {
  kfree(ptr);
}

void* operator new[](size_t sz) {
  return kalloc(sz);
}

void operator delete[](void* ptr) {
  kfree(ptr);
}
