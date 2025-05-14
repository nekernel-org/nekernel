/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

/// @file KernelTaskScheduler.h
/// @brief Kernel Task Scheduler header file.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#include <ArchKit/ArchKit.h>
#include <KernelKit/CoreProcessScheduler.h>
#include <KernelKit/LockDelegate.h>

namespace Kernel {
struct KERNEL_TASK;

struct KERNEL_TASK final {
  Char               Name[kSchedNameLen] = {"KERNEL_TASK"};
  ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemDriver};
  HAL::StackFramePtr StackFrame{nullptr};
  UInt8*             StackReserve{nullptr};
  SizeT              StackSize{kSchedMaxStackSz};
  PROCESS_IMAGE      Image{};
};
}  // namespace Kernel