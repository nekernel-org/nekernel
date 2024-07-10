/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <HALKit/POWER/Processor.hxx>
#include <KernelKit/DebugOutput.hpp>
#include <HALKit/POWER/Hart.hxx>

using namespace Kernel;

/// @brief wakes up thread.
/// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFramePtr stack)
{
	NEWOS_UNUSED(stack);
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFramePtr stack)
{
	NEWOS_UNUSED(stack);
}
