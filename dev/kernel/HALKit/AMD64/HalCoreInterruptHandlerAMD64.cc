/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/ProcessScheduler.h>
#include <NewKit/KString.h>
#include <SignalKit/Signals.h>

STATIC BOOL kIsScheduling = NO;

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp) {
  auto process = Kernel::UserProcessScheduler::The().CurrentProcess();

  if (process.Leak().Status != Kernel::ProcessStatusKind::kRunning) {
    MUST_PASS(NO);
  }

  kIsScheduling = NO;

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
  auto process = Kernel::UserProcessScheduler::The().CurrentProcess();

  if (process.Leak().Status != Kernel::ProcessStatusKind::kRunning) {
    MUST_PASS(NO);
  }

  kIsScheduling = NO;

  Kernel::kout << "Kernel: Page Fault.\r";
  Kernel::kout << "Kernel: SIGKILL\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

namespace Kernel::Detail {
constexpr static Int32 kTimeoutCount = 100000UL;
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler(Kernel::UIntPtr rsp) {
  NE_UNUSED(rsp);

  static Kernel::Int64 try_count_before_brute = Kernel::Detail::kTimeoutCount;

  while (kIsScheduling) {
    --try_count_before_brute;

    if (try_count_before_brute < 1) break;
  }

  try_count_before_brute = Kernel::Detail::kTimeoutCount;
  kIsScheduling          = YES;

  Kernel::UserProcessHelper::StartScheduling();

  kIsScheduling = NO;
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp) {
  auto process = Kernel::UserProcessScheduler::The().CurrentProcess();

  if (process.Leak().Status != Kernel::ProcessStatusKind::kRunning) {
    MUST_PASS(NO);
  }

  kIsScheduling = NO;

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
  auto process = Kernel::UserProcessScheduler::The().CurrentProcess();

  if (process.Leak().Status != Kernel::ProcessStatusKind::kRunning) {
    MUST_PASS(NO);
  }

  kIsScheduling = NO;

  Kernel::kout << "Kernel: Generic Process Fault.\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";

  process.Leak().Status = Kernel::ProcessStatusKind::kKilled;

  process.Leak().Crash();
}

EXTERN_C Kernel::Void idt_handle_breakpoint(Kernel::UIntPtr rip) {
  auto process = Kernel::UserProcessScheduler::The().CurrentProcess();

  if (process.Leak().Status != Kernel::ProcessStatusKind::kRunning) {
    (Void)(Kernel::kout << "Kernel: Kernel RIP: " << Kernel::hex_number(rip) << Kernel::kendl);
    Kernel::kout << "Kernel: SIGTRAP\r";

    kIsScheduling = NO;

    while (YES)
      ;
  }

  kIsScheduling = NO;

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
  auto process = Kernel::UserProcessScheduler::The().CurrentProcess();

  if (process.Leak().Status != Kernel::ProcessStatusKind::kRunning) {
    MUST_PASS(NO);
  }

  kIsScheduling = NO;

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
