/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef __NE_ARM64__

#include <FirmwareKit/Handover.h>
#include <NewKit/Array.h>
#include <NewKit/Defines.h>
#include <NewKit/Utils.h>

#define kCPUBackendName "aarch64"

namespace Kernel::HAL {
struct PACKED Register64 final {
  UShort  Limit;
  UIntPtr Base;
};

/// @brief Memory Manager mapping flags.
enum {
  kMMFlagsInvalid = 1 << 0,
  kMMFlagsPresent = 1 << 1,
  kMMFlagsWr      = 1 << 2,
  kMMFlagsUser    = 1 << 3,
  kMMFlagsNX      = 1 << 4,
  kMMFlagsCount   = 4,
};

/// @brief Set a PTE from pd_base.
/// @param virt_addr a valid virtual address.
/// @param phys_addr point to physical address.
/// @param flags the flags to put on the page.
/// @return Status code of page manip.
EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags);

EXTERN_C UIntPtr mm_get_phys_address(VoidPtr virtual_address);

typedef UIntPtr    Reg;
typedef Register64 Register;

/// @note let's keep the same name as AMD64 HAL.
struct PACKED StackFrame {
  Reg IP;
  Reg SP;
  Reg R8;
  Reg R9;
  Reg R10;
  Reg R11;
  Reg R12;
  Reg R13;
  Reg R14;
  Reg R15;
};

typedef StackFrame* StackFramePtr;

inline Void rt_halt() noexcept {
  while (Yes) {
  }
}

inline Void hal_wfi(Void) {
  asm volatile("wfi");
}
}  // namespace Kernel::HAL

inline Kernel::VoidPtr kKernelBitMpStart = nullptr;
inline Kernel::UIntPtr kKernelBitMpSize  = 0UL;

inline Kernel::VoidPtr kKernelPDE = nullptr;

#include <HALKit/ARM64/Paging.h>

#endif  // __NE_ARM64__