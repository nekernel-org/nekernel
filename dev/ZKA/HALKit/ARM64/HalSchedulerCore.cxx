/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.hxx>

using namespace Kernel;

Void UserProcess::SetEntrypoint(UIntPtr& imageStart) noexcept
{
	if (imageStart == 0)
		this->Crash();

	this->StackFrame->BP = imageStart;
	this->StackFrame->SP = this->StackFrame->BP;
}

namespace Kernel
{
	bool hal_check_stack(HAL::StackFramePtr stackPtr)
	{
		if (!stackPtr)
			return false;
		if (stackPtr->BP == 0 || stackPtr->SP == 0)
			return false;

		return true;
	}

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
