/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Headers/Heap.h>

typedef SizeType size_t;

void* operator new[](size_t sz) {
  if (sz == 0) ++sz;

  return RtAllocateProcessPtr(sz, kStandardAllocation);
}

void* operator new(size_t sz) {
  if (sz == 0) ++sz;

  return RtAllocateProcessPtr(sz, kArrayAllocation);
}

void operator delete[](void* ptr) {
  if (ptr == nullptr) return;

  RtFreeProcessPtr(ptr);
}