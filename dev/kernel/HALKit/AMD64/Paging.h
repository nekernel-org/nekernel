/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

/** ---------------------------------------------------

  * THIS FILE CONTAINS CODE FOR X86_64 PAGING.

------------------------------------------------------- */

#include <NewKit/Defines.h>

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
typedef VoidPtr PTE;
typedef VoidPtr PDE;
}  // namespace Kernel
