/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <NewKit/Crc32.hpp>
#include <NewKit/KernelHeap.hpp>

//! @file KernelHeap.cpp
//! @brief Kernel allocator.

#define kHeapMaxWrappers (4096 * 8)
#define kHeapMagic 0xAA55

namespace HCore {
STATIC Ref<PTEWrapper *> kWrapperList[kHeapMaxWrappers];
STATIC SizeT kHeapCount = 0UL;
STATIC Ref<PTEWrapper *> kLastWrapper;
STATIC PageManager kPageManager;

namespace Detail {
/// @brief Kernel heap information block.
/// Located before the address.
/// | HIB |                ADDRESS                |
struct HeapInformationBlock final {
  UInt16 hMagic;
  Boolean hPresent;
  Int32 hCRC32;
  Int64 hSizeAddress;
  VoidPtr hAddress;
};

typedef HeapInformationBlock *HeapInformationBlockPtr;

STATIC VoidPtr ke_find_heap(const SizeT &sz, const bool rw, const bool user) {
  for (SizeT indexWrapper = 0; indexWrapper < kHeapMaxWrappers;
       ++indexWrapper) {
    if (!kWrapperList[indexWrapper]->Present()) {
      kWrapperList[indexWrapper]
          ->Reclaim(); /* very straight-forward as you can see. */
      return reinterpret_cast<VoidPtr>(
          kWrapperList[indexWrapper]->VirtualAddress());
    }
  }

  return nullptr;
}
}  // namespace Detail

/// @brief Allocate pointer.
/// @param sz size of pointer
/// @param rw read write (true to enable it)
/// @param user is it accesible by user processes?
/// @return the pointer
VoidPtr ke_new_ke_heap(SizeT sz, const bool rw, const bool user) {
  if (sz == 0) ++sz;

  if (auto ptr = Detail::ke_find_heap(sz, rw, user); ptr) return ptr;

  Ref<PTEWrapper *> wrapper = kPageManager.Request(user, rw, false);

  Ref<PageManager> refMan(kPageManager);
  wrapper->FlushTLB(refMan);

  kLastWrapper = wrapper;

  Detail::HeapInformationBlockPtr heapInfo =
      reinterpret_cast<Detail::HeapInformationBlockPtr>(
          wrapper->VirtualAddress());

  heapInfo->hSizeAddress = sz;
  heapInfo->hMagic = kHeapMagic;
  heapInfo->hCRC32 = ke_calculate_crc32((Char *)wrapper->VirtualAddress(), sz);
  heapInfo->hAddress = (VoidPtr)wrapper->VirtualAddress();

  kWrapperList[kHeapCount] = wrapper;
  ++kHeapCount;

  return reinterpret_cast<VoidPtr>(wrapper->VirtualAddress() +
                                   sizeof(Detail::HeapInformationBlock));
}

/// @brief Declare pointer as free.
/// @param ptr the pointer.
/// @return
Int32 ke_delete_ke_heap(VoidPtr ptr) {
  if (ptr) {
    Detail::HeapInformationBlockPtr virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlockPtr>(ptr) -
        sizeof(Detail::HeapInformationBlock);

    if (kLastWrapper && virtualAddress->hMagic == kHeapMagic &&
        (UIntPtr)virtualAddress->hAddress == kLastWrapper->VirtualAddress()) {
      if (kPageManager.Free(kLastWrapper)) {
        virtualAddress->hSizeAddress = 0UL;
        virtualAddress->hPresent = false;

        return true;
      }

      return false;
    }

    Ref<PTEWrapper *> wrapper{nullptr};

    for (SizeT indexWrapper = 0; indexWrapper < kHeapCount; ++indexWrapper) {
      if (kWrapperList[indexWrapper]->VirtualAddress() ==
          (UIntPtr)virtualAddress->hAddress) {
        wrapper = kWrapperList[indexWrapper];

        // if page is no more, then mark it also as non executable.
        if (kPageManager.Free(wrapper)) {
          virtualAddress->hSizeAddress = 0UL;
          virtualAddress->hPresent = false;

          return true;
        }

        return false;
      }
    }
  }

  return -1;
}

/// @brief Check if pointer is a valid kernel pointer.
/// @param ptr the pointer
/// @return if it exists.
Boolean ke_is_valid_ptr(VoidPtr ptr) {
  if (ptr) {
    Detail::HeapInformationBlockPtr virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlockPtr>(ptr) -
        sizeof(Detail::HeapInformationBlock);

    if (virtualAddress->hPresent &&
        virtualAddress->hMagic == kHeapMagic) {
      return true;
    }
  }

  return false;
}
}  // namespace HCore
