/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
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
