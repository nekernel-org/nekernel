/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

namespace NewOS {
/// @brief wakes up thread.
/// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack) {
  HAL::rt_cli();

  stack->Rcx = 0;

  HAL::rt_sti();
}

/// @brief Hangs until RCX register is cleared.
/// @param stack 
static void __rt_hang_proc(HAL::StackFrame* stack) {
  while (stack->Rcx == 1) {
    ;
  }
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack) {
  HAL::rt_cli();

  __rt_hang_proc(stack);

  HAL::rt_sti();
}
}  // namespace NewOS
