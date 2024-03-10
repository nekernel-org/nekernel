/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <System.Core/HeapAPI.hxx>
#include <System.Core/HcHeapAPI.h>

using namespace HCore;

STATIC HcObjectPtr kObjectHeap;

HHeap* HHeap::Shared() noexcept {
    static HHeap* heap = nullptr;

    if (!heap) {
        heap = new HHeap();
        kObjectHeap = HcGetProcessHeap();
    }

    return heap;
}

void HHeap::Delete(HHeapPtr me) noexcept { HcFreeProcessHeap(kObjectHeap, me); }

SizeT HHeap::Size(HHeapPtr me) noexcept { return HcProcessHeapSize(kObjectHeap, me); }

HHeapPtr HHeap::New(const SizeT &sz, const Int32 flags) {
    SizeT _sz = sz;
    if (!_sz) ++_sz;

    return HcAllocateProcessHeap(kObjectHeap, _sz, flags); 
}