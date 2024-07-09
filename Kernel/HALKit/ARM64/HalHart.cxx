/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

namespace Kernel
{
	/// @brief wakes up thread.
	/// wakes up thread from hang.
	void rt_wakeup_thread(HAL::StackFrame* stack)
	{
		rt_do_context_switch(stack);
	}

	/// @brief makes the thread sleep on a loop.
	/// hooks and hangs thread to prevent code from executing.
	void rt_hang_thread(HAL::StackFrame* stack)
	{
		while (true)
		{
			/* nohing, code is spinning */
		}
	}
} // namespace Kernel
