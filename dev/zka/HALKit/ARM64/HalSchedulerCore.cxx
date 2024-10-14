/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.hxx>

namespace Kernel
{
   	EXTERN_C Void __zka_pure_call(void)
	{
	    UserProcessScheduler::The().CurrentProcess().Leak().Crash();
	}

	Void UserProcess::SetImageStart(VoidPtr image_start) noexcept
	{
		if (image_start == 0)
			this->Crash();

		this->Image = image_start;
	}

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
		ZKA_UNUSED(stack);
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
