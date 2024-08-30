/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

// bugs = 0

namespace Kernel
{
	/// @brief wakes up thread.
	/// wakes up thread from hang.
	void mp_wakeup_thread(HAL::StackFrame* stack)
	{
		HAL::rt_cli();

		mp_do_context_switch(stack);

		HAL::rt_sti();
	}

	/// @brief makes the thread sleep on a loop.
	/// hooks and hangs thread to prevent code from executing.
	void mp_hang_thread(HAL::StackFrame* stack)
	{
		while (true)
		{
			/* nohing, code is spinning */
		}
	}
} // namespace Kernel
