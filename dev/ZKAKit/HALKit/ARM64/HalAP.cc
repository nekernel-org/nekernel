/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Incorporated., all rights reserved.

------------------------------------------- */

#include <HALKit/ARM64/Processor.h>
#include <KernelKit/DebugOutput.h>
#include <HALKit/ARM64/AP.h>

using namespace Kernel;

namespace Kernel::Details
{
	STATIC void mp_hang_fn(void)
	{
		while (YES)
			;
	}
} // namespace Kernel::Details

/// @brief wakes up thread.
/// wakes up thread from hang.
void mp_wakeup_thread(HAL::StackFramePtr stack)
{
	if (!stack)
		return;

	hal_set_pc_to_hart(reinterpret_cast<HAL_HARDWARE_THREAD*>(stack->R15), reinterpret_cast<VoidPtr>(stack->BP));
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void mp_hang_thread(HAL::StackFramePtr stack)
{
	if (!stack)
		return;

	hal_set_pc_to_hart(reinterpret_cast<HAL_HARDWARE_THREAD*>(stack->R15), reinterpret_cast<VoidPtr>(Kernel::Details::mp_hang_fn));
}
