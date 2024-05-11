/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <KernelKit/ProcessScheduler.hpp>

using namespace NewOS;
Void ProcessHeader::SetEntrypoint(UIntPtr& imageStart) noexcept
{
	if (imageStart == 0)
		this->Crash();

	this->StackFrame->Rbp = imageStart;
	this->StackFrame->Rsp = this->StackFrame->Rbp;
}

namespace NewOS
{
	bool rt_check_stack(HAL::StackFramePtr stackPtr)
	{
		if (!stackPtr)
			return false;
		if (stackPtr->Rbp == 0 || stackPtr->Rsp == 0)
			return false;

		return true;
	}
} // namespace NewOS
