/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef __NE_AMD64__

/** ---------------------------------------------------

  * THIS FILE CONTAINS CODE FOR X86_64 PAGING.

------------------------------------------------------- */

#include <NeKit/Defines.h>

#ifndef kPageMax
#define kPageMax (0x200)
#endif  //! kPageMax

#ifndef kPageAlign
#define kPageAlign (0x08)
#endif  //! kPageAlign

#ifndef kPageSize
#define kPageSize (0x1000)
#endif  // !kPageSize

#ifndef kAlign
#define kAlign __BIGGEST_ALIGNMENT__
#endif  // !kAlign

EXTERN_C void hal_flush_tlb();
EXTERN_C void hal_invl_tlb(Kernel::VoidPtr addr);
EXTERN_C void hal_write_cr3(Kernel::VoidPtr cr3);
EXTERN_C void hal_write_cr0(Kernel::VoidPtr bit);

EXTERN_C Kernel::VoidPtr hal_read_cr0();  // @brief CPU control register.
EXTERN_C Kernel::VoidPtr hal_read_cr2();  // @brief Fault address.
EXTERN_C Kernel::VoidPtr hal_read_cr3();  // @brief Page directory inside cr3 register.

namespace Kernel::HAL {
namespace Detail {
  enum class ControlRegisterBits {
    ProtectedModeEnable = 0,
    MonitorCoProcessor  = 1,
    Emulation           = 2,
    TaskSwitched        = 3,
    ExtensionType       = 4,
    NumericError        = 5,
    WriteProtect        = 16,
    AlignementMask      = 18,
    NotWriteThrough     = 29,
    CacheDisable        = 30,
    PageEnable          = 31,
  };

  inline UInt8 control_register_cast(ControlRegisterBits reg) { return static_cast<UInt8>(reg); }
}  // namespace Detail

auto mm_alloc_bitmap(Boolean wr, Boolean user, SizeT size, Bool is_page, SizeT pad = 0) -> VoidPtr;
auto mm_free_bitmap(VoidPtr page_ptr) -> Bool;
}  // namespace Kernel::HAL

namespace Kernel {
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

struct PDE {
  ATTRIBUTE(aligned(kib_cast(4))) PTE fPTE[512];
};
}  // namespace Kernel

#endif  // __NE_AMD64__