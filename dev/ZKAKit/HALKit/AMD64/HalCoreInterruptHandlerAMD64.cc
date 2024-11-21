/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/KString.h>
#include <PosixKit/Signals.h>

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: GPF.\r";
	Kernel::UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Page Fault.\r";

	kcout << "Kernel: SIGKILL set.\r";

	auto process = Kernel::UserProcessScheduler::The().GetCurrentProcess();

	process.Leak().ProcessSignal.SignalIP		= 0UL;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kcout << "Kernel: PRCFROZE status set..\r";

	process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;

	process.Leak().Crash();
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Timer IRQ (Scheduler Notification).\r";
	Kernel::UserProcessHelper::StartScheduling();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Math error (division by zero?).\r";

	auto process = Kernel::UserProcessScheduler::The().GetCurrentProcess();

	process.Leak().ProcessSignal.SignalIP		= 0UL;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kcout << "Kernel: PRCFROZE status set..\r";

	process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;

	process.Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Generic Process Fault.\r";

	auto process = Kernel::UserProcessScheduler::The().GetCurrentProcess();

	process.Leak().ProcessSignal.SignalIP		= 0UL;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kcout << "Kernel: PRCFROZE status set..\r";

	process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;

	process.Leak().Crash();
}

EXTERN_C Kernel::Void idt_handle_breakpoint(Kernel::UIntPtr rip)
{
	auto process = Kernel::UserProcessScheduler::The().GetCurrentProcess();

	kcout << "Kernel: Process RIP: " << Kernel::number(rip) << endl;
	kcout << "Kernel: Process Name: " << process.Leak().Name << endl;

	kcout << "Kernel: SIGTRAP set.\r";

	process.Leak().ProcessSignal.SignalIP		= rip;
	process.Leak().ProcessSignal.SignalID		= SIGTRAP;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kcout << "Kernel: PRCFROZE status set..\r";

	process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Undefined Opcode.\r";

	auto process = Kernel::UserProcessScheduler::The().GetCurrentProcess();

	process.Leak().ProcessSignal.SignalIP		= 0UL;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kcout << "Kernel: PRCFROZE status set..\r";

	process.Leak().Status = Kernel::ProcessStatusKind::kFrozen;

	process.Leak().Crash();
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx_syscall_index, Kernel::UIntPtr rdx_syscall_struct)
{
	if (rcx_syscall_index < kSyscalls.Count())
	{
		kcout << "syscall: Enter Syscall.\r";

		if (kSyscalls[rcx_syscall_index].fHooked)
		{
			if (kSyscalls[rcx_syscall_index].fProc)
			{
				(kSyscalls[rcx_syscall_index].fProc)((Kernel::VoidPtr)rdx_syscall_struct);
			}
			else
			{
				kcout << "syscall: syscall isn't valid at all! (is nullptr)\r";
			}
		}
		else
		{
			kcout << "syscall: syscall isn't hooked at all! (is set to false)\r";
		}

		kcout << "syscall: Exit Syscall.\r";
	}
}

/// @brief Enter Kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx_kerncall_index, Kernel::UIntPtr rdx_kerncall_struct)
{
	if (rcx_kerncall_index < kKerncalls.Count())
	{
		kcout << "kerncall: Enter Kernel Call List.\r";

		if (kKerncalls[rcx_kerncall_index].fHooked)
		{
			if (kKerncalls[rcx_kerncall_index].fProc)
			{
				(kKerncalls[rcx_kerncall_index].fProc)((Kernel::VoidPtr)rdx_kerncall_struct);
			}
			else
			{
				kcout << "kerncall: Kernel call isn't valid at all! (is nullptr)\r";
			}
		}
		else
		{
			kcout << "kerncall: Kernel call isn't hooked at all! (is set to false)\r";
		}

		kcout << "kerncall: Exit Kernel Call List.\r";
	}
}
