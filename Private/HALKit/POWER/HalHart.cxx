/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <HALKit/POWER/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <HALKit/POWER/Hart.hxx>

using namespace NewOS;

/// @brief wakes up thread.
/// wakes up thread from hang.
void rt_wakeup_thread(HAL::StackFramePtr stack)
{
}

/// @brief makes thread sleep.
/// hooks and hangs thread to prevent code from executing.
void rt_hang_thread(HAL::StackFramePtr stack)
{
}
