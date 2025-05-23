/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/ProcessScheduler.h>
#include <NeKit/KString.h>
#include <SignalKit/Signals.h>

EXTERN_C Kernel::Void idt_handle_breakpoint(Kernel::UIntPtr rip);

EXTERN_C Kernel::UIntPtr kApicBaseAddress;

STATIC BOOL kIsRunning = NO;

/// @brief Notify APIC and PIC that we're done with the interrupt.
/// @note
STATIC void hal_idt_send_eoi(UInt8 vector) {
  ((volatile UInt32*) kApicBaseAddress)[0xB0 / 4] = 0;

  if (vector >= kPICCommand && vector <= 0x2F) {
    if (vector >= 0x28) {
      Kernel::HAL::rt_out8(kPIC2Command, kPICCommand);
    }
    Kernel::HAL::rt_out8(kPICCommand, kPICCommand);
  }

  kIsRunning = NO;
}

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C Kernel::Void idt_handle_gpf(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_idt_send_eoi(13);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_idt_send_eoi(14);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  process.Leak().Crash();
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler(Kernel::UIntPtr rsp) {
  hal_idt_send_eoi(32);

  while (kIsRunning)
    ;

  kIsRunning = YES;

  NE_UNUSED(rsp);
  Kernel::UserProcessHelper::StartScheduling();

  kIsRunning = NO;
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp) {
  hal_idt_send_eoi(8);

  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  process.Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_idt_send_eoi(30);

  Kernel::kout << "Kernel: Generic Process Fault.\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";

  process.Leak().Crash();
}

EXTERN_C Kernel::Void idt_handle_breakpoint(Kernel::UIntPtr rip) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_idt_send_eoi(3);

  process.Leak().Signal.SignalArg = rip;
  process.Leak().Signal.SignalID  = SIGTRAP;

  process.Leak().Signal.Status = process.Leak().Status;

  process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_idt_send_eoi(6);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  process.Leak().Crash();
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx_syscall_index,
                                            Kernel::UIntPtr rdx_syscall_struct) {
  hal_idt_send_eoi(50);

  if (rcx_syscall_index < kSysCalls.Count()) {
    if (kSysCalls[rcx_syscall_index].fHooked) {
      if (kSysCalls[rcx_syscall_index].fProc) {
        (kSysCalls[rcx_syscall_index].fProc)((Kernel::VoidPtr) rdx_syscall_struct);
      }
    }
  }
}

/// @brief Enter Kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx_kerncall_index,
                                            Kernel::UIntPtr rdx_kerncall_struct) {
  hal_idt_send_eoi(51);

  if (rcx_kerncall_index < kKernCalls.Count()) {
    if (kKernCalls[rcx_kerncall_index].fHooked) {
      if (kKernCalls[rcx_kerncall_index].fProc) {
        (kKernCalls[rcx_kerncall_index].fProc)((Kernel::VoidPtr) rdx_kerncall_struct);
      }
    }
  }
}
