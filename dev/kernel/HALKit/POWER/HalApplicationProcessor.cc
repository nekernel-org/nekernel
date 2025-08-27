/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/POWER/AP.h>
#include <HALKit/POWER/Processor.h>
#include <KernelKit/DebugOutput.h>

/// @note This part of the HAL needs an owner.

namespace Kernel::Detail {
STATIC void mp_hang_fn(void) {
  while (YES)
    ;
}
}  // namespace Kernel::Detail

namespace Kernel {
/// @brief wakes up thread.
/// wakes up thread from hang.
void mp_wakeup_thread(HAL::StackFramePtr stack) {
  if (!stack) return;

  MUST_PASS(stack->R15 > 0);
  MUST_PASS(stack->IP > 0);

  hal_set_pc_to_hart(reinterpret_cast<HAL_HARDWARE_THREAD*>(stack->R15),
                     reinterpret_cast<VoidPtr>(stack->IP));
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void mp_hang_thread(HAL::StackFramePtr stack) {
  if (!stack) return;

  MUST_PASS(stack->R15 > 0);

  hal_set_pc_to_hart(reinterpret_cast<HAL_HARDWARE_THREAD*>(stack->R15),
                     reinterpret_cast<VoidPtr>(Kernel::Detail::mp_hang_fn));
}
}  // namespace Kernel
