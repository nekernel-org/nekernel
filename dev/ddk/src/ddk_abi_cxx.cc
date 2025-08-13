/* -------------------------------------------

  DDK
  Copyright Amlal El Mahrouss.

  Author: Amlal El Mahrouss
  Purpose: DDK C++ ABI.

------------------------------------------- */

#include <DriverKit/ddk.h>

void* operator new(size_t sz) {
  return ::kalloc(sz);
}

void operator delete(void* ptr) {
  ::kfree(ptr);
}

void* operator new[](size_t sz) {
  return ::kalloc(sz);
}

void operator delete[](void* ptr) {
  ::kfree(ptr);
}
