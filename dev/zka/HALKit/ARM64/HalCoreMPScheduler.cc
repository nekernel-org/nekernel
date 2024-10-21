/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <modules/ACPI/ACPIFactoryInterface.h>
#include <KernelKit/UserProcessScheduler.h>

using namespace Kernel;

EXTERN_C Void mp_do_task_switch(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr stack_frame);

EXTERN_C Bool mp_register_process(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr stack_frame)
{
	mp_do_task_switch(image, stack_ptr, stack_frame);

	return Yes;
}
