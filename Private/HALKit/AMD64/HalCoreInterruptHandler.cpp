/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <NewKit/String.hpp>

static const char* kExceptionMessages[32] = {
    "Division by zero",
    "Debug Breakpoint",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Detected Overflow",
    "Out-Of-Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Found",
    "Stack Error",
    "General Protection Fault",
    "Page Fault",
    "Invalid Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

/// @brief System call interrupt (like DOS and NT)
#define kKernelSyscallInterrupt (0x21)

EXTERN_C {

HCore::UIntPtr rt_handle_interrupts(HCore::HAL::StackFramePtr sf) {
  MUST_PASS(sf);

  if (sf->IntNum < 32) {
  } else if (sf->IntNum == 0x21) {
    rt_syscall_handle(sf);
  }

  if ((sf->IntNum - 32) >= 12) {
    HCore::HAL::Out8(0xA0, 0x20);
  }

  HCore::HAL::Out8(0x20, 0x20);

  return (HCore::UIntPtr)sf;
}

}