/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/HeapMgr.h>
#include <KernelKit/KPC.h>
#include <NeKit/Crc32.h>
#include <NeKit/PageMgr.h>
#include <NeKit/Utils.h>

/* -------------------------------------------

 Revision History:
  10/8/24: FIX: Fix useless long name, alongside a new WR (WriteRead) field.
  20/10/24: FIX: Fix mm_new_ and mm_delete_ APIs inside HeapMgr.h header. (amlal)
    27/01/25: REFACTOR: Reworked code as the memory manager.
  25/03/25: REFACTOR: Refactor HeapMgr code and log freed address location.

 ------------------------------------------- */

//! @file HeapMgr.cc
//! @brief Heap system that serves as the main memory manager.

#define kMemoryMgrMagic (0xD4D75)
#define kMemoryMgrAlignSz (4U)

namespace Kernel {
/// @brief Implementation details.
namespace Detail {
  struct PACKED MM_INFORMATION_BLOCK;

  /// @brief Kernel heap information block.
  /// Located before the address bytes.
  /// | HIB |  CLASS/STRUCT/DATA TYPES... |
  struct PACKED MM_INFORMATION_BLOCK final {
    ///! @brief 32-bit value which contains the magic number of the heap.
    UInt32 fMagic : 24;

    ///! @brief Is the heap present?
    UInt8 fPresent : 1;

    /// @brief Is this value writable?
    UInt8 fWriteRead : 1;

    /// @brief Is this value owned by the user?
    UInt8 fUser : 1;

    /// @brief Is this a page pointer?
    UInt8 fPage : 1;

    /// @brief 32-bit CRC checksum.
    UInt32 fCRC32;

    /// @brief 64-bit Allocation flags.
    UInt16 fFlags;

    /// @brief 64-bit pointer size.
    SizeT fSize;

    /// @brief 64-bit target offset pointer.
    UIntPtr fOffset;

    /// @brief Padding.
    UInt32 fPad;

    /// @brief Padding bytes for header.
    UInt8 fPadding[kMemoryMgrAlignSz];
  };

  /// @brief Check for heap address validity.
  /// @param heap_ptr The address_ptr to check.
  /// @return Bool if the pointer is valid or not.
  _Output auto mm_check_ptr_address(VoidPtr heap_ptr) -> Bool {
    if (!heap_ptr) return false;

    IntPtr base_ptr = ((IntPtr) heap_ptr) - sizeof(Detail::MM_INFORMATION_BLOCK);

    /// Add that check in case we're having an integer underflow. ///

    if (base_ptr < 0) {
      return false;
    }

    return true;
  }

  typedef MM_INFORMATION_BLOCK* MM_INFORMATION_BLOCK_PTR;
}  // namespace Detail

STATIC PageMgr kPageMgr;

/// @brief Allocate chunk of memory.
/// @param sz Size of pointer
/// @param wr Read Write bit.
/// @param user User enable bit.
/// @return The newly allocated pointer.
_Output VoidPtr mm_new_ptr(SizeT sz, Bool wr, Bool user, SizeT pad_amount) {
  auto sz_fix = sz;

  if (sz_fix == 0) return nullptr;

  sz_fix += sizeof(Detail::MM_INFORMATION_BLOCK);

  auto wrapper = kPageMgr.Request(wr, user, No, sz_fix, pad_amount);

  Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
      reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>(wrapper.VirtualAddress() +
                                                         sizeof(Detail::MM_INFORMATION_BLOCK));

  heap_info_ptr->fSize  = sz_fix;
  heap_info_ptr->fMagic = kMemoryMgrMagic;
  heap_info_ptr->fCRC32 = 0U;  // dont fill it for now.
  heap_info_ptr->fOffset =
      reinterpret_cast<UIntPtr>(heap_info_ptr) + sizeof(Detail::MM_INFORMATION_BLOCK);
  heap_info_ptr->fPage      = No;
  heap_info_ptr->fWriteRead = wr;
  heap_info_ptr->fUser      = user;
  heap_info_ptr->fPresent   = Yes;
  heap_info_ptr->fPad       = pad_amount;

  rt_set_memory(heap_info_ptr->fPadding, 0, kMemoryMgrAlignSz);

  auto result = reinterpret_cast<VoidPtr>(heap_info_ptr->fOffset);

  (Void)(kout << "HeapMgr: Registered heap address: "
              << hex_number(reinterpret_cast<UIntPtr>(heap_info_ptr)) << kendl);

  return result;
}

/// @brief Makes a page heap.
/// @param heap_ptr the pointer to make a page heap.
/// @return kErrorSuccess if successful, otherwise an error code.
_Output Int32 mm_make_page(VoidPtr heap_ptr) {
  if (Detail::mm_check_ptr_address(heap_ptr) == No) return kErrorHeapNotPresent;

  Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
      reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>((UIntPtr) heap_ptr -
                                                         sizeof(Detail::MM_INFORMATION_BLOCK));

  if (!heap_info_ptr) return kErrorHeapNotPresent;

  heap_info_ptr->fPage = true;

  (Void)(kout << "HeapMgr: Registered page from heap address: "
              << hex_number(reinterpret_cast<UIntPtr>(heap_info_ptr)) << kendl);

  return kErrorSuccess;
}

/// @brief Overwrites and set the flags of a heap header.
/// @param heap_ptr the pointer to update.
/// @param flags the flags to set.
_Output Int32 mm_make_ptr_flags(VoidPtr heap_ptr, UInt64 flags) {
  if (Detail::mm_check_ptr_address(heap_ptr) == No) return kErrorHeapNotPresent;

  Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
      reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>((UIntPtr) heap_ptr -
                                                         sizeof(Detail::MM_INFORMATION_BLOCK));

  if (!heap_info_ptr) return kErrorHeapNotPresent;

  heap_info_ptr->fFlags = flags;

  return kErrorSuccess;
}

/// @brief Gets the flags of a heap header.
/// @param heap_ptr the pointer to get.
_Output UInt64 mm_get_ptr_flags(VoidPtr heap_ptr) {
  Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
      reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>((UIntPtr) heap_ptr -
                                                         sizeof(Detail::MM_INFORMATION_BLOCK));

  if (!heap_info_ptr) return kErrorHeapNotPresent;

  return heap_info_ptr->fFlags;
}

/// @brief Declare pointer as free.
/// @param heap_ptr the pointer.
/// @return
_Output Int32 mm_delete_ptr(VoidPtr heap_ptr) {
  if (Detail::mm_check_ptr_address(heap_ptr) == No) return kErrorHeapNotPresent;

  Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
      reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>((UIntPtr) (heap_ptr) -
                                                         sizeof(Detail::MM_INFORMATION_BLOCK));

  if (heap_info_ptr && heap_info_ptr->fMagic == kMemoryMgrMagic) {
    if (!heap_info_ptr->fPresent) {
      return kErrorHeapNotPresent;
    }

    heap_info_ptr->fSize      = 0UL;
    heap_info_ptr->fPresent   = No;
    heap_info_ptr->fOffset    = 0;
    heap_info_ptr->fCRC32     = 0;
    heap_info_ptr->fWriteRead = No;
    heap_info_ptr->fUser      = No;
    heap_info_ptr->fMagic     = 0;
    heap_info_ptr->fPad       = 0;

    (Void)(kout << "HeapMgr: Freed heap address: "
                << hex_number(reinterpret_cast<UIntPtr>(heap_info_ptr)) << kendl);

    PTEWrapper page_wrapper(
        No, No, No,
        reinterpret_cast<UIntPtr>(heap_info_ptr) - sizeof(Detail::MM_INFORMATION_BLOCK));

    Ref<PTEWrapper> pte_address{page_wrapper};

    kPageMgr.Free(pte_address);

    return kErrorSuccess;
  }

  return kErrorInternal;
}

/// @brief Check if pointer is a valid Kernel pointer.
/// @param heap_ptr the pointer
/// @return if it exists.
_Output Boolean mm_is_valid_ptr(VoidPtr heap_ptr) {
  if (heap_ptr && HAL::mm_is_bitmap(heap_ptr)) {
    Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
        reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>((UIntPtr) (heap_ptr) -
                                                           sizeof(Detail::MM_INFORMATION_BLOCK));

    return (heap_info_ptr && heap_info_ptr->fPresent && heap_info_ptr->fMagic == kMemoryMgrMagic);
  }

  return No;
}

/// @brief Protect the heap with a CRC value.
/// @param heap_ptr HIB pointer.
/// @return if it valid: point has crc now., otherwise fail.
_Output Boolean mm_protect_ptr(VoidPtr heap_ptr) {
  if (heap_ptr) {
    Detail::MM_INFORMATION_BLOCK_PTR heap_info_ptr =
        reinterpret_cast<Detail::MM_INFORMATION_BLOCK_PTR>((UIntPtr) heap_ptr -
                                                           sizeof(Detail::MM_INFORMATION_BLOCK));

    /// if valid, present and is heap header, then compute crc32
    if (heap_info_ptr && heap_info_ptr->fPresent && kMemoryMgrMagic == heap_info_ptr->fMagic) {
      heap_info_ptr->fCRC32 =
          ke_calculate_crc32((Char*) heap_info_ptr->fOffset, heap_info_ptr->fSize);

      return Yes;
    }
  }

  return No;
}
}  // namespace Kernel
