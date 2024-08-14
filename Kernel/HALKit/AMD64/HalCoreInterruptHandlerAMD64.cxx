/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <NewKit/String.hxx>

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp)
{
	Kernel::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp)
{
	Kernel::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp)
{
	Kernel::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp)
{
	Kernel::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp)
{
	Kernel::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
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
	if (rcx <= (kSyscalls.Count() - 1))
	{
		Kernel::kcout << "newoskrnl: kerncall: enter.\r";

		if (kKerncalls[rcx].Leak().Leak()->fHooked)
			(kKerncalls[rcx].Leak().Leak()->fProc)((Kernel::VoidPtr)rdx);

		Kernel::kcout << "newoskrnl: kerncall: exit.\r";
	}
}
