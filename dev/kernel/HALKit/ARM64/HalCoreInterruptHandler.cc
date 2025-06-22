/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/UserMgr.h>
#include <NeKit/KString.h>
#include <SignalKit/Signals.h>

EXTERN_C Kernel::Void int_handle_breakpoint(Kernel::UIntPtr rip);
EXTERN_C BOOL         mp_handle_gic_interrupt_el0(Void);

EXTERN_C BOOL  kEndOfInterrupt;
EXTERN_C UInt8 kEndOfInterruptVector;

STATIC BOOL kIsRunning = NO;

/// @note This is managed by the system software.
STATIC void hal_int_send_eoi(UInt8 vector) {
  kEndOfInterrupt       = YES;
  kEndOfInterruptVector = vector;
}

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C Kernel::Void int_handle_gpf(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_int_send_eoi(13);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void int_handle_pf(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_int_send_eoi(14);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;
}

/// @brief Handle scheduler interrupt.
EXTERN_C void int_handle_scheduler(Kernel::UIntPtr rsp) {
  NE_UNUSED(rsp);

  hal_int_send_eoi(32);

  while (kIsRunning)
    ;

  kIsRunning = YES;

  mp_handle_gic_interrupt_el0();

  kIsRunning = NO;
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void int_handle_math(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_int_send_eoi(8);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void int_handle_generic(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_int_send_eoi(30);

  Kernel::kout << "Kernel: Generic Process Fault.\r";

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;

  Kernel::kout << "Kernel: SIGKILL status.\r";
}

EXTERN_C Kernel::Void int_handle_breakpoint(Kernel::UIntPtr rip) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();

  hal_int_send_eoi(3);

  process.Leak().Signal.SignalArg = rip;
  process.Leak().Signal.SignalID  = SIGTRAP;

  process.Leak().Signal.Status = process.Leak().Status;

  process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void int_handle_ud(Kernel::UIntPtr rsp) {
  auto& process = Kernel::UserProcessScheduler::The().TheCurrentProcess();
  process.Leak().Crash();

  hal_int_send_eoi(6);

  process.Leak().Signal.SignalArg = rsp;
  process.Leak().Signal.SignalID  = SIGKILL;
  process.Leak().Signal.Status    = process.Leak().Status;
}

/// @brief Enter syscall from assembly (libSystem only)
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx_hash,
                                            Kernel::UIntPtr rdx_syscall_arg) {
  hal_int_send_eoi(50);

  if (!Kernel::kCurrentUser) return;

  for (SizeT i = 0UL; i < kMaxDispatchCallCount; ++i) {
    if (kSysCalls[i].fHooked && rcx_hash == kSysCalls[i].fHash) {
      if (kSysCalls[i].fProc) {
        (kSysCalls[i].fProc)((Kernel::VoidPtr) rdx_syscall_arg);
      }
    }
  }
}

/// @brief Enter Kernel call from assembly (libDDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx_hash,
                                            Kernel::SizeT cnt, Kernel::UIntPtr arg, Kernel::SizeT sz) {
  if (!Kernel::kRootUser) return;
  if (Kernel::kCurrentUser != Kernel::kRootUser) return;
  if (!Kernel::kCurrentUser->IsSuperUser()) return;

  for (SizeT i = 0UL; i < kMaxDispatchCallCount; ++i) {
    if (kKernCalls[i].fHooked && rcx_hash == kKernCalls[rcx_hash].fHash) {
      if (kKernCalls[i].fProc) {
        (kKernCalls[i].fProc)(cnt, (Kernel::VoidPtr) arg, sz);
      }
    }
  }
}
