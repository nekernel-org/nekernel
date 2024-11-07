/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NewKit/KString.h>

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
	Kernel::UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: IRQ0.\r";
	Kernel::UserProcessHelper::StartScheduling();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Math.\r";
	Kernel::UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Generic Fault.\r";
	Kernel::UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp)
{
	kcout << "Kernel: Undefined Opcode.\r";
	Kernel::UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
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
		kcout << "kerncall: Enter Kcall.\r";

		if (kKerncalls[rcx_kerncall_index].fHooked)
		{
			if (kKerncalls[rcx_kerncall_index].fProc)
			{
				(kKerncalls[rcx_kerncall_index].fProc)((Kernel::VoidPtr)rdx_kerncall_struct);
			}
			else
			{
				kcout << "kerncall: syscall isn't valid at all! (is nullptr)\r";
			}
		}
		else
		{
			kcout << "kerncall: syscall isn't hooked at all! (is set to false)\r";
		}

		kcout << "kerncall: Exit Kcall.\r";
	}
}
