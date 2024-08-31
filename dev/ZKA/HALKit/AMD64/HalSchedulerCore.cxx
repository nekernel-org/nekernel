/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <HALKit/AMD64/Processor.hxx>
#include <KernelKit/UserProcessScheduler.hxx>

using namespace Kernel;

Void UserProcess::SetEntrypoint(VoidPtr imageStart) noexcept
{
	if (imageStart == nullptr)
		this->Crash();

	VoidPtr cr3_pd;

	asm volatile(
		"mov %%cr3, %0"	  // Move CR3 into the variable
		: "=r"(cr3_pd) // Output operand, cr3 page directory.
		:				  // No input operands
		: "memory");

	HAL::mm_update_page(cr3_pd, 0, (UIntPtr)imageStart, HAL::eFlagsPresent | HAL::eFlagsRw | HAL::eFlagsUser);

	this->Image = imageStart;
}

namespace Kernel
{
	bool hal_check_stack(HAL::StackFramePtr stack_ptr)
	{
		if (!stack_ptr)
			return false;

		if (stack_ptr->BP == 0 || stack_ptr->SP == 0)
			return false;

		return true;
	}

	/// @brief Wakes up thread.
	/// Wakes up thread from the hang state.
	Void mp_wakeup_thread(HAL::StackFrame* stack)
	{
		mp_do_context_switch((VoidPtr)stack->BP, (UInt8*)stack->SP, stack);
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
