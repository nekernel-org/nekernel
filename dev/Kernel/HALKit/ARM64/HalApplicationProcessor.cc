/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#include <HALKit/ARM64/Processor.h>
#include <KernelKit/DebugOutput.h>
#include <HALKit/ARM64/AP.h>

using namespace Kernel;

namespace Kernel::Detail
{
	STATIC void mp_hang_fn(void)
	{
		while (YES)
			;
	}
} // namespace Kernel::Detail

/// @brief wakes up thread from it's hang state.
/// wakes up thread from hang.
Void mp_wakeup_thread(HAL::StackFramePtr stack)
{
	if (!stack)
		return;

	hal_set_pc_to_hart(reinterpret_cast<HAL_HARDWARE_THREAD*>(stack->R15), reinterpret_cast<VoidPtr>(stack->BP));
}

/// @brief makes thread go to hang state.
/// hooks and hangs thread to prevent code from executing.
Void mp_hang_thread(HAL::StackFramePtr stack)
{
	if (!stack)
		return;

	hal_set_pc_to_hart(reinterpret_cast<HAL_HARDWARE_THREAD*>(stack->R15), reinterpret_cast<VoidPtr>(Kernel::Detail::mp_hang_fn));
}
