/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/HError.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <NewKit/Crc32.hpp>
#include <NewKit/PageManager.hpp>

//! @file KernelHeap.cxx
//! @brief Kernel allocator.

#define kHeapMagic 0xD4D7

namespace HCore {
STATIC SizeT kHeapCount = 0UL;
STATIC PageManager kHeapPageManager;

namespace Detail {
/// @brief Kernel heap information block.
/// Located before the address bytes.
/// | HIB |  ADDRESS  |
struct PACKED HeapInformationBlock final {
  UInt16 hMagic;
  Boolean hPresent;
  Int32 hCRC32;
  Int64 hSizeAddress;
  UIntPtr hAddress;
};

typedef HeapInformationBlock *HeapInformationBlockPtr;
}  // namespace Detail

/// @brief allocate chunk of memory.
/// @param sz size of pointer
/// @param rw read write (true to enable it)
/// @param user is it accesible by user processes?
/// @return the pointer
VoidPtr ke_new_ke_heap(SizeT sz, const bool rw, const bool user) {
  if (sz == 0) ++sz;

  auto wrapper = kHeapPageManager.Request(rw, user, false);

  Detail::HeapInformationBlockPtr heapInfo =
      reinterpret_cast<Detail::HeapInformationBlockPtr>(
          wrapper.VirtualAddress());

  heapInfo->hSizeAddress = sz;
  heapInfo->hMagic = kHeapMagic;
  heapInfo->hCRC32 = 0;  // dont fill it for now.
  heapInfo->hAddress = wrapper.VirtualAddress();

  ++kHeapCount;

  return reinterpret_cast<VoidPtr>(wrapper.VirtualAddress() +
                                   sizeof(Detail::HeapInformationBlock));
}

/// @brief Declare pointer as free.
/// @param heapPtr the pointer.
/// @return
Int32 ke_delete_ke_heap(VoidPtr heapPtr) {
  if (kHeapCount < 1) return -kErrorInternal;

  Detail::HeapInformationBlockPtr virtualAddress =
      reinterpret_cast<Detail::HeapInformationBlockPtr>(
          (UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

  if (virtualAddress && virtualAddress->hMagic == kHeapMagic) {
    MUST_PASS(virtualAddress->hPresent);

    if (virtualAddress->hCRC32 != 0) {
      if (virtualAddress->hCRC32 !=
          ke_calculate_crc32((Char *)virtualAddress->hAddress,
                             virtualAddress->hSizeAddress))
        ke_stop(RUNTIME_CHECK_POINTER);
    }

    virtualAddress->hSizeAddress = 0UL;
    virtualAddress->hPresent = false;
    virtualAddress->hAddress = 0;
    virtualAddress->hCRC32 = 0;
    virtualAddress->hMagic = 0;

    --kHeapCount;
    return 0;
  }

  return -kErrorInternal;
}

/// @brief Check if pointer is a valid kernel pointer.
/// @param heapPtr the pointer
/// @return if it exists.
Boolean ke_is_valid_heap(VoidPtr heapPtr) {
  if (kHeapCount < 1) return false;

  if (heapPtr) {
    Detail::HeapInformationBlockPtr virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlockPtr>(
            (UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

    if (virtualAddress->hPresent && virtualAddress->hMagic == kHeapMagic) {
      return true;
    }
  }

  return false;
}

/// @brief Protect the heap pointer with a CRC32.
/// @param heapPtr 
/// @return 
Boolean ke_protect_ke_heap(VoidPtr heapPtr) {
  if (heapPtr) {
    Detail::HeapInformationBlockPtr virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlockPtr>(
            (UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

    if (virtualAddress->hPresent && virtualAddress->hMagic == kHeapMagic) {
      virtualAddress->hCRC32 = ke_calculate_crc32((Char*)heapPtr, virtualAddress->hSizeAddress);
      return true;
    }
  }

  return false;
}
}  // namespace HCore
