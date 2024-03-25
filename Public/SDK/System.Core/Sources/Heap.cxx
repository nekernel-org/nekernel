/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Headers/Heap.hxx>

/// @brief Allocate from the user's heap.
/// @param refObj Process object.
/// @param sz size of object.
/// @param flags flags.
/// @return
CA_EXTERN_C PtrVoidType HcAllocateProcessHeap(ObjectPtr refObj, QWordType sz,
                                              DWordType flags) {
  CA_MUST_PASS(sz);
  CA_MUST_PASS(flags);

  return (PtrVoidType)refObj->Invoke(refObj, kProcessCallAllocPtr, sz, flags);
}

/// @brief Free pointer from the user's heap.
/// @param refObj Process object.
/// @param ptr the pointer to free.
CA_EXTERN_C VoidType HcFreeProcessHeap(ObjectPtr refObj, PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  CA_UNREFERENCED_PARAMETER(refObj->Invoke(refObj, kProcessCallFreePtr, ptr));
}

/// @brief Get pointer size.
/// @param refObj Process object.
/// @param ptr the pointer to find.
/// @return the size.
CA_EXTERN_C QWordType HcProcessHeapSize(ObjectPtr refObj, PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  return refObj->Invoke(refObj, kProcessCallSizePtr, ptr);
}

/// @brief Check if the pointer exists.
/// @param refObj Process object.
/// @param ptr the pointer to check.
/// @return if it exists
CA_EXTERN_C BooleanType HcProcessHeapExists(ObjectPtr refObj, PtrVoidType ptr) {
  CA_MUST_PASS(ptr);
  return refObj->Invoke(refObj, kProcessCallCheckPtr, ptr);
}

using namespace System;

/// @brief Shared instance of the heap.
/// @return
HeapInterface* HeapInterface::Shared() noexcept {
  static HeapInterface* heap = nullptr;

  if (!heap) {
    heap = new HeapInterface();
  }

  return heap;
}

HeapInterface::HeapInterface() {
  CA_MUST_PASS(HcProcessHeapExists(kApplicationObject, (PtrVoidType)this));
}

HeapInterface::~HeapInterface() { delete this; }

void HeapInterface::Delete(PtrHeapType me) noexcept {
  CA_MUST_PASS(me);
  HcFreeProcessHeap(kApplicationObject, me);
}

SizeType HeapInterface::Size(PtrHeapType me) noexcept {
  CA_MUST_PASS(me);
  return HcProcessHeapSize(kApplicationObject, me);
}

PtrHeapType HeapInterface::New(const SizeType& sz, const DWordType flags) {
  SizeType _sz = sz;
  if (!_sz) ++_sz;

  return HcAllocateProcessHeap(kApplicationObject, _sz, flags);
}
