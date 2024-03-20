/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <NewKit/Crc32.hpp>
#include <NewKit/PageManager.hpp>

//! @file KernelHeap.cxx
//! @brief Kernel allocator.

#define kHeapMagic 0xD4D7

namespace HCore {
STATIC SizeT kHeapCount = 0UL;
STATIC Ref<PTEWrapper> kHeapLastWrapper;
STATIC PageManager kHeapPageManager;

namespace Detail {
/// @brief Kernel heap information block.
/// Located before the address bytes.
/// | HIB |  ADDRESS  |
struct HeapInformationBlock final {
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
  kHeapLastWrapper = wrapper;

  kcout << "HCoreKrnl.exe: Populating HIB...\r\n";

  Detail::HeapInformationBlockPtr heapInfo =
      reinterpret_cast<Detail::HeapInformationBlockPtr>(
          wrapper.VirtualAddress());

  heapInfo->hSizeAddress = sz;
  heapInfo->hMagic = kHeapMagic;
  heapInfo->hCRC32 = 0;  // dont fill it for now.
  heapInfo->hAddress = wrapper.VirtualAddress();

  ++kHeapCount;

  kcout << "HCoreKrnl.exe: Return address...\r\n";

  return reinterpret_cast<VoidPtr>(wrapper.VirtualAddress() +
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

    if (kHeapLastWrapper && virtualAddress->hMagic == kHeapMagic &&
        virtualAddress->hAddress == kHeapLastWrapper.Leak().VirtualAddress()) {
      virtualAddress->hSizeAddress = 0UL;
      virtualAddress->hPresent = false;

      --kHeapCount;

      return true;
    }
  }

  return -1;
}

/// @brief Check if pointer is a valid kernel pointer.
/// @param ptr the pointer
/// @return if it exists.
Boolean ke_is_valid_ptr(VoidPtr ptr) {
  if (kHeapCount < 1) return false;

  if (ptr) {
    Detail::HeapInformationBlockPtr virtualAddress =
        reinterpret_cast<Detail::HeapInformationBlockPtr>(ptr) -
        sizeof(Detail::HeapInformationBlock);

    if (virtualAddress->hPresent && virtualAddress->hMagic == kHeapMagic) {
      return true;
    }
  }

  return false;
}
}  // namespace HCore
