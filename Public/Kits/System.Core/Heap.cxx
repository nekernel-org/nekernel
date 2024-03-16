/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/Heap.hxx>
#include <System.Core/System.Core.hxx>

using namespace HCore;
using namespace System;

STATIC HcObjectPtr kObjectHeap;

Heap* Heap::Shared() noexcept {
    static Heap* heap = nullptr;

    if (!heap) {
        heap = new Heap();
        kObjectHeap = HcGetProcessHeap();
    }

    return heap;
}

void Heap::Delete(HeapPtr me) noexcept { HcFreeProcessHeap(kObjectHeap, me); }

SizeT Heap::Size(HeapPtr me) noexcept { 
    CA_MUST_PASS(me);
    return HcProcessHeapSize(kObjectHeap, me); 
}

HeapPtr Heap::New(const SizeT &sz, const Int32 flags) {
    SizeT _sz = sz;
    if (!_sz) ++_sz;

    return HcAllocateProcessHeap(kObjectHeap, _sz, flags); 
}