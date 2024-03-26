/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Headers/Heap.hxx>

typedef SizeType size_t;

void* operator new[](size_t sz) {
  if (sz == 0) ++sz;

  return RtAllocateProcessHeap(kApplicationObject, sz, kStandardAllocation);
}

void* operator new(size_t sz) {
  if (sz == 0) ++sz;

  return RtAllocateProcessHeap(kApplicationObject, sz, kArrayAllocation);
}

void operator delete[](void* ptr) {
  if (ptr == nullptr) return;

  RtFreeProcessHeap(kApplicationObject, ptr);
}

void operator delete(void* ptr) {
  if (ptr == nullptr) return;

  RtFreeProcessHeap(kApplicationObject, ptr);
}

void operator delete(void* ptr, size_t sz) {
  if (ptr == nullptr) return;

  (void)sz;

  RtFreeProcessHeap(kApplicationObject, ptr);
}
