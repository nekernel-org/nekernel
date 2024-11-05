/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <HALKit/AXP/Processor.h>

/// @brief Internal call for syscall, to work with C++.
/// @param stack
/// @return nothing.
EXTERN_C void rt_syscall_handle(Kernel::HAL::StackFrame* stack)
{
	if (stack->Rcx <= (kSyscalls.Count() - 1))
	{
		kcout << "syscall: enter.\r";

		if (kSyscalls[stack->Rcx].Leak().Leak().fHooked)
			(kSyscalls[stack->Rcx].Leak().Leak().fProc)(stack);

		kcout << "syscall: exit.\r";
	}
}
