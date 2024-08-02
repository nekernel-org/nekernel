/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <HALKit/POWER/Processor.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <HALKit/POWER/Hart.hxx>

using namespace Kernel;

/// @brief wakes up thread.
/// wakes up thread from hang.
void mp_wakeup_thread(HAL::StackFramePtr stack)
{
	NEWOS_UNUSED(stack);
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void mp_hang_thread(HAL::StackFramePtr stack)
{
	NEWOS_UNUSED(stack);
}
