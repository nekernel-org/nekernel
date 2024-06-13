/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <HALKit/AXP/Processor.hpp>

/// @brief Internal call for syscall, to work with C++.
/// @param stack 
/// @return nothing.
EXTERN_C void rt_syscall_handle(NewOS::HAL::StackFrame* stack)
{
	if (stack->Rcx <= (kSyscalls.Count() - 1))
	{
		NewOS::kcout << "newoskrnl: syscall: enter.\r";

		if (kSyscalls[stack->Rcx].Leak().Leak().fHooked)
			(kSyscalls[stack->Rcx].Leak().Leak().fProc)(stack);

		NewOS::kcout << "newoskrnl: syscall: exit.\r";
	}
}
