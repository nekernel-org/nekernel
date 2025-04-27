/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: HalPagingMgr.cc
  Purpose: Platform Paging Manager.

------------------------------------------- */

#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Processor.h>

namespace Kernel::HAL {
namespace Detail {
  /// @brief Page Table Entry for AMD64.
  struct PTE {
    UInt64 Present : 1;
    UInt64 Wr : 1;
    UInt64 User : 1;
    UInt64 Pwt : 1;  // Page-level Write-Through
    UInt64 Pcd : 1;  // Page-level Cache Disable
    UInt64 Accessed : 1;
    UInt64 Dirty : 1;
    UInt64 Pat : 1;  // Page Attribute Table (or PS for PDE)
    UInt64 Global : 1;
    UInt64 Ignored1 : 3;          // Available to software
    UInt64 PhysicalAddress : 40;  // Physical page frame address (bits 12–51)
    UInt64 Ignored2 : 7;          // More software bits / reserved
    UInt64 ProtectionKey : 4;     // Optional (if PKU enabled)
    UInt64 Reserved : 1;          // Usually reserved
    UInt64 Nx : 1;                // No Execute
  };
}  // namespace Detail

/***********************************************************************************/
/// \brief Retrieve the page status of a PTE.
/// \param pte Page Table Entry pointer.
/***********************************************************************************/
STATIC Void mmi_page_status(Detail::PTE* pte) {
  (Void)(kout << (pte->Present ? "Present" : "Not Present") << kendl);
  (Void)(kout << (pte->Wr ? "W/R" : "Not W/R") << kendl);
  (Void)(kout << (pte->Nx ? "NX" : "Not NX") << kendl);
  (Void)(kout << (pte->User ? "User" : "Not User") << kendl);
  (Void)(kout << (pte->Pcd ? "Not Cached" : "Cached") << kendl);
  (Void)(kout << (pte->Accessed ? "Accessed" : "Not Accessed") << kendl);
  (Void)(kout << hex_number(pte->PhysicalAddress) << kendl);
  (Void)(kout << (pte->ProtectionKey ? "Protected" : "Not Protected/PKU Disabled") << kendl);
}

/***********************************************************************************/
/// @brief Gets a physical address from a virtual address.
/// @param virt a valid virtual address.
/// @return Physical address.
/***********************************************************************************/
UIntPtr hal_get_phys_address(VoidPtr virt) {
  const UInt64 kVMAddr         = (UInt64) virt;
  const UInt64 kMask9Bits      = 0x1FFULL;
  const UInt64 kPageOffsetMask = 0xFFFULL;

  UInt64 cr3 = (UInt64) hal_read_cr3() & ~kPageOffsetMask;

  // Level 4
  auto   pml4  = reinterpret_cast<UInt64*>(cr3);
  UInt64 pml4e = pml4[(kVMAddr >> 39) & kMask9Bits];

  if (!(pml4e & 1)) return 0;

  // Level 3
  auto   pdpt  = reinterpret_cast<UInt64*>(pml4e & ~kPageOffsetMask);
  UInt64 pdpte = pdpt[(kVMAddr >> 30) & kMask9Bits];

  if (!(pdpte & 1)) return 0;

  // Level 2
  auto   pd  = reinterpret_cast<UInt64*>(pdpte & ~kPageOffsetMask);
  UInt64 pde = pd[(kVMAddr >> 21) & kMask9Bits];

  if (!(pde & 1)) return 0;

  // 1 GiB page support
  if (pde & (1 << 7)) {
    return (pde & ~((1ULL << 30) - 1)) | (kVMAddr & ((1ULL << 30) - 1));
  }

  // Level 1
  auto         pt  = reinterpret_cast<UInt64*>(pde & ~kPageOffsetMask);
  Detail::PTE* pte = (Detail::PTE*) pt[(kVMAddr >> 12) & kMask9Bits];

  if (!pte->Present) return 0;

  mmi_page_status((Detail::PTE*) pte);

  return (pte->PhysicalAddress << 12) | (kVMAddr & 0xFFF);
}

/***********************************************************************************/
/// @brief Maps or allocates a page from virtual_address.
/// @param virtual_address a valid virtual address.
/// @param phys_addr point to physical address.
/// @param flags the flags to put on the page.
/// @return Status code of page manipulation process.
/***********************************************************************************/
EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags) {
  const UInt64     kVMAddr   = (UInt64) virtual_address;
  constexpr UInt64 kMask9    = 0x1FF;
  constexpr UInt64 kPageMask = 0xFFF;

  UInt64 cr3 = (UIntPtr) hal_read_cr3() & ~kPageMask;

  auto   pml4  = reinterpret_cast<UInt64*>(cr3);
  UInt64 pml4e = pml4[(kVMAddr >> 39) & kMask9];

  if (!(pml4e & 1)) return kErrorInvalidData;

  UInt64* pdpt  = reinterpret_cast<UInt64*>(pml4e & ~kPageMask);
  UInt64  pdpte = pdpt[(kVMAddr >> 30) & kMask9];

  if (!(pdpte & 1)) return kErrorInvalidData;

  UInt64* pd  = reinterpret_cast<UInt64*>(pdpte & ~kPageMask);
  UInt64  pde = pd[(kVMAddr >> 21) & kMask9];

  if (!(pde & 1)) return kErrorInvalidData;

  UInt64*      pt  = reinterpret_cast<UInt64*>(pde & ~kPageMask);
  Detail::PTE* pte = (Detail::PTE*) pt[(kVMAddr >> 12) & kMask9];

  pte->Present = !!(flags & kMMFlagsPresent);
  pte->Wr      = !!(flags & kMMFlagsWr);
  pte->User    = !!(flags & kMMFlagsUser);
  pte->Nx      = !!(flags & kMMFlagsNX);
  pte->Pcd     = !!(flags & kMMFlagsPCD);
  pte->Pwt     = !!(flags & kMMFlagsPwt);

  pte->PhysicalAddress = ((UIntPtr) (physical_address)) >> 12;

  hal_invl_tlb(virtual_address);

  asm volatile("clflush (%0)" : : "r"(virtual_address) : "memory");

  asm volatile("mfence" ::: "memory");

  mmi_page_status(pte);

  return kErrorSuccess;
}
}  // namespace Kernel::HAL
