/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/KString.h>
#include <SignalKit/Signals.h>

STATIC BOOL kIsScheduling = NO;

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(NeOS::UIntPtr rsp)
{
	auto process = NeOS::UserProcessScheduler::The().CurrentProcess();

	if (process.Leak().Status != NeOS::ProcessStatusKind::kRunning)
		return;

	kIsScheduling = NO;

	kout << "Kernel: General Protection Fault.\r";

	process.Leak().ProcessSignal.SignalArg		= rsp;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kout << "Kernel: SIGKILL status.\r";

	process.Leak().Status = NeOS::ProcessStatusKind::kKilled;

	process.Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(NeOS::UIntPtr rsp)
{
	auto process = NeOS::UserProcessScheduler::The().CurrentProcess();

	if (process.Leak().Status != NeOS::ProcessStatusKind::kRunning)
		return;

	kIsScheduling = NO;

	kout << "Kernel: Page Fault.\r";
	kout << "Kernel: SIGKILL\r";

	process.Leak().ProcessSignal.SignalArg		= rsp;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	process.Leak().Status = NeOS::ProcessStatusKind::kKilled;

	process.Leak().Crash();
}

namespace NeOS::Detail
{
	constexpr static Int32 kTimeoutCount = 100000UL;
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler(NeOS::UIntPtr rsp)
{
	static NeOS::Int64 try_count_before_brute = NeOS::Detail::kTimeoutCount;

	while (kIsScheduling)
	{
		--try_count_before_brute;

		if (try_count_before_brute < 1)
			break;
	}

	try_count_before_brute = NeOS::Detail::kTimeoutCount;
	kIsScheduling		   = YES;

	NeOS::UserProcessHelper::StartScheduling();

	kIsScheduling = NO;
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(NeOS::UIntPtr rsp)
{
	auto process = NeOS::UserProcessScheduler::The().CurrentProcess();

	if (process.Leak().Status != NeOS::ProcessStatusKind::kRunning)
		return;

	kIsScheduling = NO;

	kout << "Kernel: Math error (division by zero?).\r";

	process.Leak().ProcessSignal.SignalArg		= rsp;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kout << "Kernel: SIGKILL status.\r";

	process.Leak().Status = NeOS::ProcessStatusKind::kKilled;

	process.Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(NeOS::UIntPtr rsp)
{
	auto process = NeOS::UserProcessScheduler::The().CurrentProcess();

	if (process.Leak().Status != NeOS::ProcessStatusKind::kRunning)
	{
		kout << "Getting here.\r";
		return;
	}

	kIsScheduling = NO;

	kout << "Kernel: Generic Process Fault.\r";

	process.Leak().ProcessSignal.SignalArg		= rsp;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kout << "Kernel: SIGKILL status.\r";

	process.Leak().Status = NeOS::ProcessStatusKind::kKilled;

	process.Leak().Crash();
}

EXTERN_C NeOS::Void idt_handle_breakpoint(NeOS::UIntPtr rip)
{
	auto process = NeOS::UserProcessScheduler::The().CurrentProcess();

	if (process.Leak().Status != NeOS::ProcessStatusKind::kRunning)
	{
		kout << "Kernel: SIGTRAP\r";

		while (YES)
			;
	}

	kIsScheduling = NO;

	kout << "Kernel: Process RIP: " << NeOS::hex_number(rip) << kendl;
	kout << "Kernel: SIGTRAP\r";

	process.Leak().ProcessSignal.SignalArg = rip;
	process.Leak().ProcessSignal.SignalID  = SIGTRAP;

	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kout << "Kernel: SIGTRAP status.\r";

	process.Leak().Status = NeOS::ProcessStatusKind::kFrozen;
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(NeOS::UIntPtr rsp)
{
	auto process = NeOS::UserProcessScheduler::The().CurrentProcess();

	if (process.Leak().Status != NeOS::ProcessStatusKind::kRunning)
		return;

	kIsScheduling = NO;

	kout << "Kernel: Undefined Opcode.\r";

	process.Leak().ProcessSignal.SignalArg		= rsp;
	process.Leak().ProcessSignal.SignalID		= SIGKILL;
	process.Leak().ProcessSignal.PreviousStatus = process.Leak().Status;

	kout << "Kernel: SIGKILL status.\r";

	process.Leak().Status = NeOS::ProcessStatusKind::kKilled;

	process.Leak().Crash();
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C NeOS::Void hal_system_call_enter(NeOS::UIntPtr rcx_syscall_index, NeOS::UIntPtr rdx_syscall_struct)
{
	if (rcx_syscall_index < kSyscalls.Count())
	{
		kout << "syscall: Enter Syscall.\r";

		if (kSyscalls[rcx_syscall_index].fHooked)
		{
			if (kSyscalls[rcx_syscall_index].fProc)
			{
				(kSyscalls[rcx_syscall_index].fProc)((NeOS::VoidPtr)rdx_syscall_struct);
			}
			else
			{
				kout << "syscall: syscall isn't valid at all! (is nullptr)\r";
			}
		}
		else
		{
			kout << "syscall: syscall isn't hooked at all! (is set to false)\r";
		}

		kout << "syscall: Exit Syscall.\r";
	}
}

/// @brief Enter Kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C NeOS::Void hal_kernel_call_enter(NeOS::UIntPtr rcx_kerncall_index, NeOS::UIntPtr rdx_kerncall_struct)
{
	if (rcx_kerncall_index < kKerncalls.Count())
	{
		kout << "kerncall: Enter Kernel Call List.\r";

		if (kKerncalls[rcx_kerncall_index].fHooked)
		{
			if (kKerncalls[rcx_kerncall_index].fProc)
			{
				(kKerncalls[rcx_kerncall_index].fProc)((NeOS::VoidPtr)rdx_kerncall_struct);
			}
			else
			{
				kout << "kerncall: Kernel call isn't valid at all! (is nullptr)\r";
			}
		}
		else
		{
			kout << "kerncall: Kernel call isn't hooked at all! (is set to false)\r";
		}

		kout << "kerncall: Exit Kernel Call List.\r";
	}
}
