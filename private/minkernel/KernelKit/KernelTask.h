// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#ifndef KERNELKIT_KERNELTASKSCHEDULER_H
#define KERNELKIT_KERNELTASKSCHEDULER_H

/// @file KernelTaskScheduler.h
/// @brief Ne::Kernel Task Scheduler backend file.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#include <ArchKit/ArchKit.h>
#include <KernelKit/CoreProcessScheduler.h>
#include <KernelKit/LockDelegate.h>

namespace Ne::Kernel {

class KernelTaskHelper;

using KID = ProcessID;

/// @brief Equivalent of UserProcess, but for kernel tasks.
/// @author Amlal
class KernelTask final {
 public:
  Char               Name[kCPSNameLen] = {"KernelTask"};
  ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemKernel};
  HAL::StackFramePtr StackFrame{nullptr};
  UInt8*             StackReserve{nullptr};
  SizeT              StackSize{kCPSMaxStackSz};
  ProcessImage       Image{};

  /// @brief a KID is a Ne::Kernel ID, it is used to find a task running within the kernel.
  KID Kid{};

  NE_NON_VETTABLE;
};

/// @brief Equivalent of UserProcessHelper, but for kernel tasks.
/// @author Amlal
class KernelTaskHelper final {
 public:
  STATIC Bool Start(KernelTask& task_ref, const KID& kid);
  STATIC Bool CanBeStarted(const KernelTask& task_ref);
  STATIC Bool IsValid(const KernelTask& task_ref);

  NE_NON_VETTABLE;
};

}  // namespace Ne::Kernel

#endif
