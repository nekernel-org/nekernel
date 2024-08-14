/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <HALKit/AXP/Processor.hxx>

/// @brief Internal call for syscall, to work with C++.
/// @param stack
/// @return nothing.
EXTERN_C void rt_syscall_handle(Kernel::HAL::StackFrame* stack)
{
	if (stack->Rcx <= (kSyscalls.Count() - 1))
	{
		Kernel::kcout << "newoskrnl: syscall: enter.\r";

		if (kSyscalls[stack->Rcx].Leak().Leak().fHooked)
			(kSyscalls[stack->Rcx].Leak().Leak().fProc)(stack);

		Kernel::kcout << "newoskrnl: syscall: exit.\r";
	}
}
