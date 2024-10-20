/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include "NewKit/Stop.hxx"
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/String.hxx>

namespace Kernel
{
	EXTERN UserProcessScheduler* cProcessScheduler;
}

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(Kernel::UIntPtr rsp)
{
	if (Kernel::cProcessScheduler == nullptr)
	{
		Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
	}

	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(Kernel::UIntPtr rsp)
{
	if (Kernel::cProcessScheduler == nullptr)
	{
		Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
	}

	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
}

/// @brief Handle scheduler interrupt.
EXTERN_C void idt_handle_scheduler()
{
	Kernel::UserProcessHelper::StartScheduling();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(Kernel::UIntPtr rsp)
{
	if (Kernel::cProcessScheduler == nullptr)
	{
		Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
	}

	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();

	Kernel::UserProcessHelper::StartScheduling();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(Kernel::UIntPtr rsp)
{
	if (Kernel::cProcessScheduler == nullptr)
	{
		Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
	}

	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(Kernel::UIntPtr rsp)
{
	if (Kernel::cProcessScheduler == nullptr)
	{
		Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
	}

	Kernel::UserProcessScheduler::The().CurrentProcess().Leak().Crash();

	Kernel::UserProcessHelper::StartScheduling();
	Kernel::ke_stop(RUNTIME_CHECK_PROCESS);
}

/// @brief Enter syscall from assembly.
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_system_call_enter(Kernel::UIntPtr rcx, Kernel::UIntPtr rdx)
{
	Kernel::HAL::Out8(0x20, 0x20); // Acknowledge interrupt to master PIC

	if (rcx <= (kSyscalls.Count() - 1))
	{
		kcout << "syscall: Enter Fn.\r";

		if (kSyscalls[rcx].fHooked)
			(kSyscalls[rcx].fProc)((Kernel::VoidPtr)rdx);

		kcout << "syscall: Exit Fn.\r";
	}
}

/// @brief Enter Kernel call from assembly (DDK only).
/// @param stack the stack pushed from assembly routine.
/// @return nothing.
EXTERN_C Kernel::Void hal_kernel_call_enter(Kernel::UIntPtr rcx, Kernel::UIntPtr rdx, Kernel::UIntPtr r8, Kernel::UIntPtr r9)
{
	Kernel::HAL::Out8(0x20, 0x20); // Acknowledge interrupt to master PIC

	if (rcx <= (kSyscalls.Count() - 1))
	{
		kcout << "kerncall: Enter Fn.\r";

		if (kKerncalls[rcx].fHooked)
			(kKerncalls[rcx].fProc)((Kernel::VoidPtr)rdx);

		kcout << "kerncall: Exit Fn.\r";
	}
}
