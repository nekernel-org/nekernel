/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

namespace NewOS {
// @brief wakes up thread.
// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack) {
  HAL::rt_cli();

    // TODO

  HAL::rt_sti();
}

static void __rt_hang_proc(void) {
  while (1) {

  }
}

// @brief makes thread sleep.
// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack) {
  HAL::rt_cli();

    // TODO

  HAL::rt_sti();
}
}  // namespace NewOS
