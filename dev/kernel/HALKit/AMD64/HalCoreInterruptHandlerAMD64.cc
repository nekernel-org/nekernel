/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/ProcessScheduler.h>
#include <NewKit/KString.h>
#include <SignalKit/Signals.h>

EXTERN_C Kernel::Void idt_handle_breakpoint(Kernel::UIntPtr rip);

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().CurrentProcess();

  Kernel::kout << "Kernel: General Protection Fault.\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().CurrentProcess();

  Kernel::kout << "Kernel: Page Fault.\r";
  Kernel::kout << "Kernel: SIGKILL\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler(Kernel::UIntPtr rsp) {
  NE_UNUSED(rsp);
  Kernel::UserProcessHelper::StartScheduling();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().CurrentProcess();

  Kernel::kout << "Kernel: Math error (division by zero?).\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().CurrentProcess();

  (Void)(Kernel::kout << "Kernel: Process RSP: " << Kernel::hex_number(rsp) << Kernel::kendl);
  Kernel::kout << "Kernel: Access Process Fault.\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

EXTERN_C Kernel::Void idt_handle_breakpoint(Kernel::UIntPtr rip) {
  auto& process = Kernel::UserProcessScheduler::The().CurrentProcess();

  (Void)(Kernel::kout << "Kernel: Process RIP: " << Kernel::hex_number(rip) << Kernel::kendl);

  Kernel::kout << "Kernel: SIGTRAP\r";

  process.Leak().Signal.SignalArg = rip;
  process.Leak().Signal.SignalID  = SIGTRAP;

  process.Leak().Signal.Status = process.Leak().Status;

  Kernel::kout << "Kernel: SIGTRAP status.\r";

  process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp) {
  NE_UNUSED(rsp);

  auto& process = Kernel::UserProcessScheduler::The().CurrentProcess();

  Kernel::kout << "Kernel: Undefined Opcode.\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx_syscall_index,
                                            Kernel::UIntPtr rdx_syscall_struct) {
  if (rcx_syscall_index < kSysCalls.Count()) {
    Kernel::kout << "syscall: Enter Syscall.\r";

    if (kSysCalls[rcx_syscall_index].fHooked) {
      if (kSysCalls[rcx_syscall_index].fProc) {
        (kSysCalls[rcx_syscall_index].fProc)((Kernel::VoidPtr) rdx_syscall_struct);
      } else {
        Kernel::kout << "syscall: syscall isn't valid at all! (is nullptr)\r";
      }
    } else {
      Kernel::kout << "syscall: syscall isn't hooked at all! (is set to false)\r";
    }

    Kernel::kout << "syscall: Exit Syscall.\r";
  }
}

/// @brief Enter Kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx_kerncall_index,
                                            Kernel::UIntPtr rdx_kerncall_struct) {
  if (rcx_kerncall_index < kKernCalls.Count()) {
    Kernel::kout << "kerncall: Enter Kernel Call List.\r";

    if (kKernCalls[rcx_kerncall_index].fHooked) {
      if (kKernCalls[rcx_kerncall_index].fProc) {
        (kKernCalls[rcx_kerncall_index].fProc)((Kernel::VoidPtr) rdx_kerncall_struct);
      } else {
        Kernel::kout << "kerncall: Kernel call isn't valid at all! (is nullptr)\r";
      }
    } else {
      Kernel::kout << "kerncall: Kernel call isn't hooked at all! (is set to false)\r";
    }

    Kernel::kout << "kerncall: Exit Kernel Call List.\r";
  }
}
