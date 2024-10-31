/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <KernelKit/Timer.h>

///! BUGS: 0
///! @file Timer.cc
///! @brief Software Timer implementation

using namespace Kernel;

/// @brief Unimplemented as it is an interface.
Int32 TimerInterface::Wait() noexcept
{
	return kErrorUnimplemented;
}

/// @brief SoftwareTimer class, meant to be generic.

SoftwareTimer::SoftwareTimer(Int64 seconds)
	: fWaitFor(seconds)
{
	fDigitalTimer = new IntPtr();
	MUST_PASS(fDigitalTimer);
}

SoftwareTimer::~SoftwareTimer()
{
	delete fDigitalTimer;
	fWaitFor = 0;
}

Int32 SoftwareTimer::Wait() noexcept
{
	if (fWaitFor < 1)
		return 1;

	while (*fDigitalTimer < (*fDigitalTimer + fWaitFor))
	{
		++(*fDigitalTimer);
	}

	return 0;
}
