/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <NewKit/String.hxx>

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp)
{
	Kernel::kcout
		<< "newoskrnl: General Protection Fault, caused by "
		<< Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().GetProcessName();

	Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp)
{
	Kernel::kcout
		<< "newoskrnl: Segmentation Fault, caused by "
		<< Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().GetProcessName();

	Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp)
{
	Kernel::kcout
		<< "newoskrnl: Math error, caused by "
		<< Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().GetProcessName();

	Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp)
{
	Kernel::kcout
		<< "newoskrnl: Execution error, caused by "
		<< Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().GetProcessName();

	Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp)
{
	Kernel::kcout
		<< "newoskrnl: Invalid interrupt, caused by "
		<< Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().GetProcessName();

	Kernel::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx, Kernel::UIntPtr rdx)
{
	if (rcx <= (kSyscalls.Count() - 1))
	{
		Kernel::kcout << "newoskrnl: syscall: enter.\r";

		if (kSyscalls[rcx].Leak().Leak()->fHooked)
			(kSyscalls[rcx].Leak().Leak()->fProc)((Kernel::VoidPtr)rdx);

		Kernel::kcout << "newoskrnl: syscall: exit.\r";
	}
}

/// @brief Enter kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx, Kernel::UIntPtr rdx, Kernel::UIntPtr r8, Kernel::UIntPtr r9)
{
	if (rcx != 0)
	{
		Kernel::kcout << "newoskrnl: kerncall: enter.\r";

		Kernel::kcout << "newoskrnl: kerncall: exit.\r";
	}
}
