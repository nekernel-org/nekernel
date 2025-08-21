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
class KernelTaskHelper;

typedef PID KID;

/// @brief Equivalent of USER_PROCESS, but for kernel tasks.
/// @author Amlal
class KERNEL_TASK final {
 public:
  Char               Name[kSchedNameLen] = {"KERNEL_TASK"};
  ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemDriver};
  HAL::StackFramePtr StackFrame{nullptr};
  UInt8*             StackReserve{nullptr};
  SizeT              StackSize{kSchedMaxStackSz};
  ProcessImage      Image{};
  /// @brief a KID is a Kernel Identification Descriptor, it is used to find a task running within
  /// the kernel.
  KID Kid{0};
};

/// @brief Equivalent of UserProcessHelper, but for kernel tasks.
/// @author Amlal
class KernelTaskHelper final {
 public:
  STATIC Bool Switch(HAL::StackFramePtr frame_ptr, PID new_kid);
  STATIC Bool CanBeScheduled(const KERNEL_TASK& process);
  STATIC ErrorOr<PID> TheCurrentKID();
  STATIC SizeT        StartScheduling();
};
}  // namespace Kernel