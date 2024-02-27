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
    "Non-maskable interrupt",
    "Breakpoint",
    "Detected overflow",
    "Out-of-bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not found",
    "Stack error.",
    "General Protection Fault",
    "Page Fault",
    "Invalid interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
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

extern "C" {

HCore::UIntPtr rt_handle_interrupts(HCore::HAL::StackFramePtr sf) {
  HCore::HAL::rt_cli();

  if (sf->IntNum < 32) {
  } else if (sf->IntNum == 0x21) {
  }

  if ((sf->IntNum - 32) >= 12) {
    HCore::HAL::Out8(0xA0, 0x20);
  }

  HCore::HAL::Out8(0x20, 0x20);

  HCore::HAL::rt_sti();

  return (HCore::UIntPtr)sf;
}

}