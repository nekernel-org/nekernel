/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/Timer.h>

/// @brief SoftwareTimer class, meant to be generic.

using namespace Kernel;

SoftwareTimer::SoftwareTimer(Int64 seconds)
	: fWaitFor(seconds)
{
	fDigitalTimer = new IntPtr();
	MUST_PASS(fDigitalTimer);
}

SoftwareTimer::~SoftwareTimer()
{
	delete fDigitalTimer;
	fDigitalTimer = nullptr;

	fWaitFor = 0;
}

BOOL SoftwareTimer::Wait() noexcept
{
	if (fWaitFor < 1)
		return NO;

	while (*fDigitalTimer < (*fDigitalTimer + fWaitFor))
	{
		++(*fDigitalTimer);
	}

	return YES;
}
