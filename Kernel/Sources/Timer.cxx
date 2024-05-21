/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <KernelKit/Timer.hpp>

///! BUGS: 0
///! @file Timer.cxx

using namespace NewOS;

/// @brief Unimplemented as it is an interface.
Int32 HardwareTimerInterface::Wait() noexcept
{
	return kErrorUnimplemented;
}

/// @brief HardwareTimer class, meant to be generic.

HardwareTimer::HardwareTimer(Int64 seconds)
	: fWaitFor(seconds)
{
}
HardwareTimer::~HardwareTimer()
{
	fWaitFor = 0;
}

Int32 HardwareTimer::Wait() noexcept
{
	if (fWaitFor < 1)
		return -1;

	while (*fDigitalTimer < (*fDigitalTimer + fWaitFor))
	{
	}

	return 0;
}
