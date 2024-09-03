/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/String.hxx>

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp)
{
	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp)
{
	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp)
{
	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp)
{
	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp)
{
	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx, Kernel::UIntPtr rdx)
{
	if (rcx <= (kSyscalls.Count() - 1))
	{
		Kernel::kcout << "newoskrnl.exe: syscall: enter.\r";

		if (kSyscalls[rcx].fHooked)
			(kSyscalls[rcx].fProc)((Kernel::VoidPtr)rdx);

		Kernel::kcout << "newoskrnl.exe: syscall: exit.\r";
	}
}

/// @brief Enter Kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx, Kernel::UIntPtr rdx, Kernel::UIntPtr r8, Kernel::UIntPtr r9)
{
	if (rcx <= (kSyscalls.Count() - 1))
	{
		Kernel::kcout << "newoskrnl.exe: kerncall: enter.\r";

		if (kKerncalls[rcx].fHooked)
			(kKerncalls[rcx].fProc)((Kernel::VoidPtr)rdx);

		Kernel::kcout << "newoskrnl.exe: kerncall: exit.\r";
	}
}
