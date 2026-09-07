// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#include <KernelKit/CodeMgr.h>
#include <KernelKit/KPC.h>
#include <KernelKit/KernelTask.h>

/***********************************************************************************/
/// @file KernelTaskScheduler.cpp
/// @brief Ne::Kernel Task scheduler.
/// @author Amlal El Mahrouss (amlal@nekernel.org)
/***********************************************************************************/

namespace Ne::Kernel {

/// @internal @brief Calls the DDK stub to initialize the stack_frame of the driver.
EXTERN_C Int32 kt_kernel_task_start(HAL::StackFramePtr stack_frame, VoidPtr code) {
  MUST_PASS(stack_frame && code);

  if (!stack_frame || !code)
    ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR, "Ne::Kernel task arguments are invalid");

  ((rtl_kstart_kind) (code))(stack_frame);

  if (!stack_frame->R8) ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR, "Ne::Kernel task failed to run");

  return stack_frame->R8;
}

Bool KernelTaskHelper::Start(KernelTask& task_ptr, const KID& kid) {
  if (!kid) return NO;

  STATIC Bool kLocked{NO};

  while (kLocked);

  kLocked = YES;

  task_ptr.Kid = kid;

  auto ret = kt_kernel_task_start(task_ptr.StackFrame, task_ptr.Image.LeakImage().Leak().Leak());

  kLocked = NO;

  if (ret != kErrorSuccess) {
    err_local_get() = ret;
    return FALSE;
  }

  return TRUE;
}

Bool KernelTaskHelper::CanBeStarted(const KernelTask& task) {
  if (KernelTaskHelper::IsValid(task)) return YES;
  return task.StackFrame != nullptr;
}

Bool KernelTaskHelper::IsValid(const KernelTask& task_ref) {
  if (task_ref.StackSize == 0) return NO;

  return task_ref.Image.HasImage() && task_ref.Image.HasCode();
}

}  // namespace Ne::Kernel
