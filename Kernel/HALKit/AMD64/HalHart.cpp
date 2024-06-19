/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

namespace NewOS
{
	/// @brief wakes up thread.
	/// wakes up thread from hang.
	void rt_wakeup_thread(HAL::StackFrame* stack)
	{
		HAL::rt_cli();

		rt_do_context_switch(stack);

		HAL::rt_sti();
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
} // namespace NewOS
