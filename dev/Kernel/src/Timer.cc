/* -------------------------------------------

	Copyright (C) 2024, t& Labs, all rights reserved.

------------------------------------------- */

#include <KernelKit/Timer.h>

///! BUGS: 0
///! @file Timer.cc
///! @brief Software Timer implementation

using namespace Kernel;

/// @brief Unimplemented as it is an interface.
BOOL TimerInterface::Wait() noexcept
{
	return NO;
}
