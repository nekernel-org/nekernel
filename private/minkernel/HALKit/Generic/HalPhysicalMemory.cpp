// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#include <KernelKit/DebugOutput.h>
#include <HALKit/Generic/PhysicalMemory.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/Utils.h>

/// @file HalPhysicalMemory.cpp
/// @brief Physical frame allocator, the memory page tables are built from.

namespace Ne::Kernel::HAL {

namespace Detail {

  /// @brief Bump cursor, everything above it inside the region is untouched.
  STATIC UIntPtr kPmmBase{0UL};
  STATIC UIntPtr kPmmCursor{0UL};
  STATIC UIntPtr kPmmEnd{0UL};

  /// @brief Freed frames, threaded through the frames themselves.
  STATIC UIntPtr kPmmFreeHead{0UL};

  STATIC SizeT kPmmFree{0UL};
  STATIC SizeT kPmmUsed{0UL};

  inline constexpr auto kPmmAlign = kPageSize - 1;
  
}  // namespace Detail

/// @brief Hand a physical region to the frame allocator.
/// @param base first byte of the region.
/// @param sz size of the region in bytes.
Void pmmi_init(UIntPtr base, SizeT sz) {
  auto start = (base + Detail::kPmmAlign) & ~Detail::kPmmAlign;
  auto end   = (base + sz) & ~Detail::kPmmAlign;

  if (end <= start) {
    Detail::kPmmCursor = 0UL;
    Detail::kPmmEnd    = 0UL;

    return;
  }

  Detail::kPmmBase     = start;
  Detail::kPmmCursor   = start;
  Detail::kPmmEnd      = end;
  Detail::kPmmFreeHead = 0UL;
  Detail::kPmmFree     = (end - start) / kPageSize;
  Detail::kPmmUsed     = 0UL;
}

/// @brief Take one frame.
/// @return the frame's physical address, zeroed, or 0 when out of memory.
_Output UIntPtr pmmi_alloc_frame(Void) {
  UIntPtr frame = 0UL;

  STATIC std::atomic_flag kLocked = ATOMIC_FLAG_INIT;

  while (kLocked.test_and_set(std::memory_order_acquire));

  if (Detail::kPmmFreeHead) {
    frame = Detail::kPmmFreeHead;

    Detail::kPmmFreeHead = *reinterpret_cast<UIntPtr*>(frame);
  } else if (Detail::kPmmCursor && Detail::kPmmCursor < Detail::kPmmEnd) {
    frame = Detail::kPmmCursor;
    Detail::kPmmCursor += kPageSize;
  }

  if (!frame) {
    kLocked.clear(std::memory_order_release);
    return 0UL;
  }

  rt_set_memory(reinterpret_cast<VoidPtr>(frame), 0, kPageSize);

  --Detail::kPmmFree;
  ++Detail::kPmmUsed;

  kLocked.clear(std::memory_order_release);

  return frame;
}

/// @brief Give a frame back.
/// @param frame the frame's physical address.
Void pmmi_free_frame(UIntPtr frame) {
  if (!frame || (frame & Detail::kPmmAlign)) return;

  if (frame < Detail::kPmmBase || frame >= Detail::kPmmCursor) return;

  STATIC std::atomic_flag kLocked = ATOMIC_FLAG_INIT;

  while (kLocked.test_and_set(std::memory_order_acquire));

  *reinterpret_cast<UIntPtr*>(frame) = Detail::kPmmFreeHead;

  Detail::kPmmFreeHead = frame;

  ++Detail::kPmmFree;
  --Detail::kPmmUsed;

  kLocked.clear(std::memory_order_release);
}

/// @brief Frames still available.
_Output SizeT pmmi_free_frames(Void) {
  return Detail::kPmmFree;
}

/// @brief Frames handed out.
_Output SizeT pmmi_used_frames(Void) {
  return Detail::kPmmUsed;
}

}  // namespace Ne::Kernel::HAL
