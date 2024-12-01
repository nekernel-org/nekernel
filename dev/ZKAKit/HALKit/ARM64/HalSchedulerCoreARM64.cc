/* -------------------------------------------

	Copyright (C) 2024, ELMH Group, all rights reserved.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	EXTERN_C Void __zka_pure_call(void)
	{
		UserProcessScheduler::The().GetCurrentProcess().Leak().Crash();
	}

	bool hal_check_stack(HAL::StackFramePtr stackPtr)
	{
		if (!stackPtr)
			return No;

		if (stackPtr->BP == 0 || stackPtr->SP == 0)
			return No;

		return Yes;
	}

	/// @brief Wakes up thread.
	/// Wakes up thread from the hang state.
	Void mp_wakeup_thread(HAL::StackFrame* stack)
	{
		ZKA_UNUSED(stack);
	}

	/// @brief makes the thread sleep on a loop.
	/// hooks and hangs thread to prevent code from executing.
	Void mp_hang_thread(HAL::StackFrame* stack)
	{
		ZKA_UNUSED(stack);
	}
} // namespace Kernel
