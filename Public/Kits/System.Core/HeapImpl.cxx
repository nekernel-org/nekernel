/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Heap.hxx>
#include <System.Core/System.Core.hxx>

using namespace System;

/// @brief Shared instance of the heap.
/// @return 
Heap* Heap::Shared() noexcept {
  static Heap* heap = nullptr;

  if (!heap) {
    heap = new Heap();
  }

  return heap;
}

Heap::~Heap() { delete this; }

void Heap::Delete(HeapPtr me) noexcept {
  CA_MUST_PASS(me);
  HcFreeProcessHeap(kInstanceObject, me);
}

SizeT Heap::Size(HeapPtr me) noexcept {
  CA_MUST_PASS(me);
  return HcProcessHeapSize(kInstanceObject, me);
}

HeapPtr Heap::New(const SizeT& sz, const Int32 flags) {
  SizeT _sz = sz;
  if (!_sz) ++_sz;

  return HcAllocateProcessHeap(kInstanceObject, _sz, flags);
}