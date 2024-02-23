/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

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
/// @brief
struct HeapInformationBlock final {
  Int16 hMagic;
  Boolean hPresent;
  Int64 hSize;
  Int32 hCRC32;
  VoidPtr hPtr;
};

STATIC voidPtr ke_find_heap(const SizeT &sz, const bool rw, const bool user) {
  for (SizeT indexWrapper = 0; indexWrapper < kHeapMaxWrappers;
       ++indexWrapper) {
    if (!kWrapperList[indexWrapper]->Present()) {
      kWrapperList[indexWrapper]
          ->Reclaim(); /* very straight-forward as you can see. */
      return reinterpret_cast<voidPtr>(
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

  kLastWrapper = wrapper;

  Detail::HeapInformationBlock *heapInfo =
      reinterpret_cast<Detail::HeapInformationBlock *>(
          wrapper->VirtualAddress());

  heapInfo->hSize = sz;
  heapInfo->hMagic = kHeapMagic;
  heapInfo->hCRC32 = ke_calculate_crc32((Char *)wrapper->VirtualAddress(), sz);
  heapInfo->hPtr = (VoidPtr)wrapper->VirtualAddress();

  kWrapperList[kHeapCount] = wrapper;
  ++kHeapCount;

  return reinterpret_cast<voidPtr>(wrapper->VirtualAddress() +
                                   sizeof(Detail::HeapInformationBlock));
}

/// @brief Declare pointer as free.
/// @param ptr the pointer.
/// @return
Int32 ke_delete_ke_heap(voidPtr ptr) {
  if (ptr) {
    Detail::HeapInformationBlock *virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlock *>(ptr) -
        sizeof(Detail::HeapInformationBlock);

    if (kLastWrapper &&
        (UIntPtr)virtualAddress->hPtr == kLastWrapper->VirtualAddress()) {
      if (kPageManager.Free(kLastWrapper)) {
        virtualAddress->hSize = 0UL;
        virtualAddress->hPresent = false;
        kLastWrapper->NoExecute(false);
        return true;
      }

      return false;
    }

    Ref<PTEWrapper *> wrapper{nullptr};

    for (SizeT indexWrapper = 0; indexWrapper < kHeapCount; ++indexWrapper) {
      if (kWrapperList[indexWrapper]->VirtualAddress() ==
          (UIntPtr)virtualAddress->hPtr) {
        wrapper = kWrapperList[indexWrapper];

        // if page is no more, then mark it also as non executable.
        if (kPageManager.Free(wrapper)) {
          virtualAddress->hSize = 0UL;
          virtualAddress->hPresent = false;

          wrapper->NoExecute(false);

          return true;
        }

        return false;
      }
    }
  }

  return -1;
}

/// @brief find pointer in kernel heap
/// @param ptr the pointer
/// @return if it exists.
Boolean kernel_valid_ptr(voidPtr ptr) {
  if (ptr) {
    const UIntPtr virtualAddress = reinterpret_cast<UIntPtr>(ptr);

    if (kLastWrapper &&
        virtualAddress == (kLastWrapper->VirtualAddress() +
                           sizeof(Detail::HeapInformationBlock))) {
      return true;
    }

    Ref<PTEWrapper *> wrapper;

    for (SizeT indexWrapper = 0; indexWrapper < kHeapCount; ++indexWrapper) {
      if ((kLastWrapper->VirtualAddress() +
           sizeof(Detail::HeapInformationBlock)) == virtualAddress) {
        wrapper = kWrapperList[indexWrapper];
        return true;
      }
    }
  }

  return false;
}
}  // namespace HCore
