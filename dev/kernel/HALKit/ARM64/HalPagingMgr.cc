/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: HalPagingMgr.cc
  Purpose: Platform Paging Manager.

------------------------------------------- */

#include <HALKit/ARM64/Paging.h>
#include <HALKit/ARM64/Processor.h>

namespace Kernel::HAL {
typedef UInt32 PageTableIndex;

EXTERN_C UIntPtr mm_get_page_addr(VoidPtr virtual_address) {
  if (!virtual_address) return 0;

  UInt64 ttbr0_val = 0;

  asm volatile("mrs %0, ttbr0_el1" : "=r"(ttbr0_val));
  volatile UInt64* l1_table = reinterpret_cast<volatile UInt64*>(ttbr0_val);

  UInt64 l1_idx = (reinterpret_cast<UIntPtr>(virtual_address) >> 39) & 0x1FF;
  UInt64 l2_idx = (reinterpret_cast<UIntPtr>(virtual_address) >> 30) & 0x1FF;
  UInt64 l3_idx = (reinterpret_cast<UIntPtr>(virtual_address) >> 21) & 0x1FF;

  if (!l1_table[l1_idx]) return 0;

  volatile UInt64* l2_table = reinterpret_cast<volatile UInt64*>(l1_table[l1_idx] & ~0xFFFUL);

  if (!l2_table[l2_idx]) return 0;

  volatile UInt64* l3_table = reinterpret_cast<volatile UInt64*>(l2_table[l2_idx] & ~0xFFFUL);

  if (!l3_table[l3_idx]) return 0;

  return (l3_table[l3_idx] & ~0xFFFUL);
}

/// @brief Maps or allocates a page from virtual_address.
/// @param virtual_address a valid virtual address.
/// @param phys_addr point to physical address.
/// @param flags the flags to put on the page.
/// @return Status code of page manipulation process.
EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags,
                           UInt32 level) {
  if (!virtual_address || !flags || !physical_address) return kErrorInvalidData;

  UInt64 ttbr0_val = 0;

  asm volatile("mrs %0, ttbr0_el1" : "=r"(ttbr0_val));
  volatile UInt64* l1_table = reinterpret_cast<volatile UInt64*>(ttbr0_val);

  UInt64 l1_idx = (reinterpret_cast<UIntPtr>(virtual_address) >> 39) & 0x1FF;
  UInt64 l2_idx = (reinterpret_cast<UIntPtr>(virtual_address) >> 30) & 0x1FF;
  UInt64 l3_idx = (reinterpret_cast<UIntPtr>(virtual_address) >> 21) & 0x1FF;

  if (!l1_table[l1_idx]) return kErrorInvalidData;

  volatile UInt64* l2_table = reinterpret_cast<volatile UInt64*>(l1_table[l1_idx] & ~0xFFFUL);

  if (!l2_table[l2_idx]) return kErrorInvalidData;

  volatile UInt64* l3_table = reinterpret_cast<volatile UInt64*>(l2_table[l2_idx] & ~0xFFFUL);

  l3_table[l3_idx] = (reinterpret_cast<UInt64>(physical_address) & ~0xFFFUL) | flags;

  switch (level) {
    case 2: {
      l3_table[l3_idx] = (reinterpret_cast<UInt64>(physical_address) & ~0xFFFUL) | flags;
      return kErrorSuccess;
    }
    case 1: {
      l1_table[l1_idx] = (reinterpret_cast<UInt64>(physical_address) & ~0xFFFUL) | flags;
      return kErrorSuccess;
    }
    case 0: {
      l1_table[l1_idx] = (reinterpret_cast<UInt64>(physical_address) & ~0xFFFUL) | flags;
      return kErrorSuccess;
    }
  }

  return kErrorInvalidData;
}
}  // namespace Kernel::HAL
