/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Headers/Heap.hxx>
#include <System.Core/Headers/Heap.hxx>

#define kAllocationTypes 2

enum HcAllocationKind {
  kStandardAllocation = 0xC,
  kArrayAllocation = 0xD,
};

CA_EXTERN_C PtrVoidType HcAllocateProcessHeap(ObjectPtr refObj, QWordType sz,
                                              DWordType flags);
CA_EXTERN_C BooleanType HcProcessHeapExists(ObjectPtr refObj, PtrVoidType ptr);
CA_EXTERN_C QWordType HcProcessHeapSize(ObjectPtr refObj, PtrVoidType ptr);
CA_EXTERN_C VoidType HcFreeProcessHeap(ObjectPtr refObj, PtrVoidType ptr);

typedef SizeType size_t;

void* operator new[](size_t sz) {
  if (sz == 0) ++sz;

  return HcAllocateProcessHeap(kApplicationObject, sz, kStandardAllocation);
}

void* operator new(size_t sz) {
  if (sz == 0) ++sz;

  return HcAllocateProcessHeap(kApplicationObject, sz, kArrayAllocation);
}

void operator delete[](void* ptr) {
  if (ptr == nullptr) return;

  HcFreeProcessHeap(kApplicationObject, ptr);
}

void operator delete(void* ptr) {
  if (ptr == nullptr) return;

  HcFreeProcessHeap(kApplicationObject, ptr);
}

void operator delete(void* ptr, size_t sz) {
  if (ptr == nullptr) return;

  (void)sz;

  HcFreeProcessHeap(kApplicationObject, ptr);
}
