/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#include <HALKit/POWER/Processor.h>
#include <KernelKit/DebugOutput.h>
#include <HALKit/POWER/Hart.h>

using namespace Kernel;

/// @brief wakes up thread.
/// wakes up thread from hang.
void mp_wakeup_thread(HAL::StackFramePtr stack)
{
	ZKA_UNUSED(stack);
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void mp_hang_thread(HAL::StackFramePtr stack)
{
	ZKA_UNUSED(stack);
}
