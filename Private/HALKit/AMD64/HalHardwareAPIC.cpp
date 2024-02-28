/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

namespace HCore {
// @brief wakes up thread.
// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack) {
  HAL::rt_cli();

    // TODO

  HAL::rt_sti();
}

static void __rt_hang_proc(void) {
  while (1)
    ;
}

// @brief makes thread sleep.
// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack) {
  __asm__ volatile("cli");

    // TODO

  __asm__ volatile("sti");
}
}  // namespace HCore
