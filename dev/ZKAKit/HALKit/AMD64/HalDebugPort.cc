/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

//! @file DebuggerPort.cc
//! @brief UART debug via packets.

#include <ArchKit/ArchKit.h>
#include <KernelKit/DebugOutput.h>

// after that we have start of additional data.

namespace Kernel
{
	void rt_debug_listen(DebuggerPortHeader* theHook) noexcept
	{
		if (theHook == nullptr)
			return;

		for (UInt32 i = 0U; i < kDebugMaxPorts; ++i)
		{
			HAL::rt_out16(theHook->fPort[i], kDebugMag0);
			HAL::rt_wait_400ns();

			HAL::rt_out16(theHook->fPort[i], kDebugMag1);
			HAL::rt_wait_400ns();

			HAL::rt_out16(theHook->fPort[i], kDebugMag2);
			HAL::rt_wait_400ns();

			HAL::rt_out16(theHook->fPort[i], kDebugMag3);
			HAL::rt_wait_400ns();

			if (HAL::rt_in16(theHook->fPort[i] != kDebugUnboundPort))
				++theHook->fBoundCnt;
		}
	}
} // namespace Kernel
