/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/AMD64/Processor.h>
#include <KernelKit/ProcessScheduler.h>

namespace Kernel {
/***********************************************************************************/
/// @brief Unimplemented function (crashes by default)
/// @param
/***********************************************************************************/

EXTERN_C Void __ne_pure_call(USER_PROCESS* process) {
  if (process) process->Crash();
}

/***********************************************************************************/
/// @brief Validate user stack.
/// @param stack_ptr the frame pointer.
/***********************************************************************************/

EXTERN_C Bool hal_check_task(HAL::StackFramePtr stack_ptr) {
  if (!stack_ptr) return No;

  return stack_ptr->SP > 0 && stack_ptr->IP > 0;
}

/// @brief Wakes up thread.
/// Wakes up thread from the hang state.
Void mp_wakeup_thread(HAL::StackFrame* stack) {
  if (!hal_check_task(stack)) return;

  // RIP is always in R15. R15 is reserved for the RIP.
  stack->IP = stack->R15;

  Kernel::UserProcessHelper::StartScheduling();
}

/// @brief makes the thread sleep on a loop.
/// hooks and hangs thread to prevent code from executing.
Void mp_hang_thread(HAL::StackFrame* stack) {
  if (!hal_check_task(stack)) return;

  // Store IP in R15
  stack->R15 = stack->IP;
  stack->IP  = 0UL;
}
}  // namespace Kernel
