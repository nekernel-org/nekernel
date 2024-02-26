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

static const char* kExceptionMessage[32] = {
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
    "System Process Switch Issued",
    "System was interrupted by kernel",
    "System hang by kernel",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

extern "C" HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr& rsp) {
  HCore::HAL::rt_cli();

  HCore::HAL::StackFramePtr sf = (HCore::HAL::StackFramePtr)rsp;

  if (sf->IntNum == 0x21) {
    rt_syscall_handle(sf);
  }

  if (sf->IntNum < 32) {
    HCore::kcout << "Exception:" << kExceptionMessage[sf->IntNum] << "\n";
  }

  if (sf->IntNum >= 40) HCore::HAL::Out8(0x20, 0x20);  // ACK MASTER

  HCore::HAL::Out8(0xA0, 0x20);  // ACK SLAVE

  HCore::HAL::rt_sti();

  return rsp;
}
