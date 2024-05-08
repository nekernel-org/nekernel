/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <HALKit/POWER/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

EXTERN_C NewOS::HAL::StackFramePtr rt_get_current_context()
{
	return nullptr;
}
