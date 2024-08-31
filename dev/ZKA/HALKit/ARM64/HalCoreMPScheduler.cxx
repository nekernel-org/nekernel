/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

// bugs = 0

namespace Kernel
{
	/// @brief Wakes up thread.
	/// Wakes up thread from the hang state.
	Void mp_wakeup_thread(HAL::StackFrame* stack)
	{
		mp_do_context_switch(stack);
	}

	/// @brief makes the thread sleep on a loop.
	/// hooks and hangs thread to prevent code from executing.
	Void mp_hang_thread(HAL::StackFrame* stack)
	{
		while (Yes)
		{
			/* Nothing to do, code is spinning */
		}
	}
} // namespace Kernel
