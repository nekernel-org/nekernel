/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <NewKit/PageManager.hpp>
#include <KernelKit/HError.hpp>
#include <NewKit/Crc32.hpp>

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
/// @param ptr the pointer.
/// @return
Int32 ke_delete_ke_heap(VoidPtr ptr) {
  if (kHeapCount < 1) return -kErrorInternal;

  Detail::HeapInformationBlockPtr virtualAddress =
      reinterpret_cast<Detail::HeapInformationBlockPtr>((UIntPtr)ptr -
      sizeof(Detail::HeapInformationBlock));

  if (virtualAddress && virtualAddress->hMagic == kHeapMagic) {
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
/// @param ptr the pointer
/// @return if it exists.
Boolean ke_is_valid_ptr(VoidPtr ptr) {
  if (kHeapCount < 1) return false;

  if (ptr) {
    Detail::HeapInformationBlockPtr virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlockPtr>((UIntPtr)ptr -
      sizeof(Detail::HeapInformationBlock));

    if (virtualAddress->hPresent && virtualAddress->hMagic == kHeapMagic) {
      return true;
    }
  }

  return false;
}
}  // namespace HCore
