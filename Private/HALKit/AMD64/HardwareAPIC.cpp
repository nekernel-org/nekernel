/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>

// bugs = 0

namespace HCore {
// @brief wakes up thread.
// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFrame* stack) {
  __asm__ volatile("cli");

  stack->Rbp = stack->R15;
  stack->Rsi = stack->Rbp;

  __asm__ volatile("sti");
}

static void __rt_hang_proc(void) {
  while (1)
    ;
}

// @brief makes thread sleep.
// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFrame* stack) {
  __asm__ volatile("cli");

  stack->R15 = stack->Rbp;
  stack->Rbp = (HAL::Reg)&__rt_hang_proc;
  stack->Rsp = stack->Rbp;

  __asm__ volatile("sti");
}
}  // namespace HCore
