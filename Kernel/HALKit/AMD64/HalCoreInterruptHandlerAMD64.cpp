/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hxx>
#include <NewKit/String.hpp>

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(NewOS::UIntPtr rsp)
{
	NewOS::kcout
		<< "newoskrnl: General Protection Fault, caused by "
		<< NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(NewOS::UIntPtr rsp)
{
	NewOS::kcout
		<< "newoskrnl: Segmentation Fault, caused by "
		<< NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(NewOS::UIntPtr rsp)
{
	NewOS::kcout
		<< "newoskrnl: Math error, caused by "
		<< NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(NewOS::UIntPtr rsp)
{
	NewOS::kcout
		<< "newoskrnl: Execution error, caused by "
		<< NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(NewOS::UIntPtr rsp)
{
	NewOS::kcout
		<< "newoskrnl: Invalid interrupt, caused by "
		<< NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C NewOS::Void hal_system_call_enter(NewOS::UIntPtr rcx, NewOS::UIntPtr rdx)
{
	if (rcx <= (kSyscalls.Count() - 1))
	{
		NewOS::kcout << "newoskrnl: syscall: enter.\r";

		if (kSyscalls[rcx].Leak().Leak()->fHooked)
			(kSyscalls[rcx].Leak().Leak()->fProc)((NewOS::VoidPtr)rdx);

		NewOS::kcout << "newoskrnl: syscall: exit.\r";
	}
}

/// @brief Enter kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C NewOS::Void hal_kernel_call_enter(NewOS::UIntPtr rcx, NewOS::UIntPtr rdx, NewOS::UIntPtr r8, NewOS::UIntPtr r9)
{
	if (rcx != 0)
	{
		NewOS::kcout << "newoskrnl: kerncall: enter.\r";

		NewOS::kcout << "newoskrnl: kerncall: exit.\r";
	}
}
