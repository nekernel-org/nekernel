/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <HALKit/AMD64/Processor.hxx>
#include <KernelKit/UserProcessScheduler.hxx>

using namespace Kernel;

Void UserProcess::SetImageStart(VoidPtr imageStart) noexcept
{
	if (imageStart == nullptr)
		this->Crash();

	HAL::mm_update_pte(hal_read_cr3(), 0, imageStart, HAL::eFlagsPresent | HAL::eFlagsUser);

	this->Image = imageStart;
}

namespace Kernel
{
	bool hal_check_stack(HAL::StackFramePtr stack_ptr)
	{
		if (!stack_ptr)
			return false;

		return true;
	}

	/// @brief Wakes up thread.
	/// Wakes up thread from the hang state.
	Void mp_wakeup_thread(HAL::StackFrame* stack)
	{
		Kernel::UserProcessHelper::StartScheduling();
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
