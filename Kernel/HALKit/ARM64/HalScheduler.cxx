/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/ProcessScheduler.hxx>

using namespace Kernel;

Void ProcessHeader::SetEntrypoint(UIntPtr& imageStart) noexcept
{
	if (imageStart == 0)
		this->Crash();

	this->StackFrame->BP = imageStart;
	this->StackFrame->SP = this->StackFrame->BP;
}

namespace Kernel
{
	bool rt_check_stack(HAL::StackFramePtr stackPtr)
	{
		if (!stackPtr)
			return false;
		if (stackPtr->BP == 0 || stackPtr->SP == 0)
			return false;

		return true;
	}
} // namespace Kernel
