/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <HALKit/POWER/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

extern "C" NewOS::HAL::StackFramePtr rt_get_current_context()
{
	return nullptr;
}
