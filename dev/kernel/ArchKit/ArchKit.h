/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Array.h>
#include <NeKit/Defines.h>
#include <NeKit/Function.h>

#include <FirmwareKit/Handover.h>

#ifdef __NE_AMD64__
#include <HALKit/AMD64/Hypervisor.h>
#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Processor.h>
#elif defined(__NE_POWER64__)
#include <HALKit/POWER/Processor.h>
#elif defined(__NE_ARM64__)
#include <HALKit/ARM64/Processor.h>
#elif defined(__NE_SDK__)
#include <HALKit/ARM64/Processor.h>
#else
#error !!! unknown architecture !!!
#endif

#define kMaxDispatchCallCount (512U)

namespace Kernel {
inline SizeT rt_hash_seed(const Char* seed, UInt32 mul) {
  SizeT hash = 0;

  for (SSizeT idx = 0; seed[idx] != 0; ++idx) {
    hash += seed[idx];
    hash ^= mul;
  }

  return hash;
}

/// @brief write to mapped memory register
/// @param base the base address.
/// @param reg the register.
/// @param value the write to write on it.
template <typename DataKind>
inline Void ke_dma_write(UIntPtr base, DataKind reg, DataKind value) noexcept {
  *(volatile DataKind*) (base + reg) = value;
}

/// @brief read from mapped memory register.
/// @param base base address
/// @param reg the register.
/// @return the value inside the register.
template <typename DataKind>
inline UInt32 ke_dma_read(UIntPtr base, DataKind reg) noexcept {
  return *(volatile DataKind*) (base + reg);
}

/// @brief Hardware Abstraction Layer
namespace HAL {
  /// @brief Check whether this pointer is a bitmap object.
  /// @param ptr argument to verify.
  /// @param whether successful or not.
  auto mm_is_bitmap(VoidPtr ptr) -> Bool;
}  // namespace HAL
}  // namespace Kernel

typedef Kernel::Void (*rt_syscall_proc)(Kernel::VoidPtr);

/// @brief System Call Dispatch.
struct HAL_DISPATCH_ENTRY final {
  Kernel::UInt64  fHash;
  Kernel::Bool    fHooked;
  rt_syscall_proc fProc;

  operator bool() { return fHooked; }
};

typedef Kernel::Void (*rt_kerncall_proc)(Kernel::SizeT, Kernel::VoidPtr, Kernel::SizeT);

/// @brief Kernel Call Dispatch.
struct HAL_KERNEL_DISPATCH_ENTRY final {
  Kernel::UInt64   fHash;
  Kernel::Bool     fHooked;
  rt_kerncall_proc fProc;

  operator bool() { return fHooked; }
};

inline Kernel::Array<HAL_DISPATCH_ENTRY, kMaxDispatchCallCount> kSysCalls;

inline Kernel::Array<HAL_KERNEL_DISPATCH_ENTRY, kMaxDispatchCallCount> kKernCalls;

#ifdef __NE_VIRTUAL_MEMORY_SUPPORT__

inline Kernel::VoidPtr kKernelVM = nullptr;

#endif  // __NE_VIRTUAL_MEMORY_SUPPORT__

inline Kernel::SizeT kBitMapCursor = 0UL;
