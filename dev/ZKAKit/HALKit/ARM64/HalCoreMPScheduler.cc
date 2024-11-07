/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#include <Modules/ACPI/ACPIFactoryInterface.h>
#include <KernelKit/UserProcessScheduler.h>

using namespace Kernel;

STATIC struct PROCESS_CONTROL_BLOCK final
{
	HAL::StackFramePtr f_Frame;
	UInt8*			   f_Stack;
	VoidPtr			   f_Image;
} kProcessBlocks[kSchedProcessLimitPerTeam] = {0};

EXTERN_C HAL::StackFramePtr mp_get_current_context(Void)
{
	return kProcessBlocks[UserProcessScheduler::The().GetCurrentProcess().Leak().ProcessId % kSchedProcessLimitPerTeam].f_Frame;
}

EXTERN_C Bool mp_register_process(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr stack_frame)
{
	MUST_PASS(image && stack_ptr && stack_frame);
	return No;
}
