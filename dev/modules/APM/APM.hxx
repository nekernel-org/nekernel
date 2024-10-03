/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

namespace Kernel
{
	typedef Int32 APMPowerCmd;

	enum
	{
		kAPMPowerCommandStop	= 0x01,
		kAPMPowerCommandStart	= 0x02,
		kAPMPowerCommandSleep	= 0x04,
		kAPMPowerCommandSWakeup = 0x06
	};

	/// @brief Send a APM command into it's own DMA space.
	/// @param base_dma the DMA base address.
	/// @param cmd the command.
	/// @return status code.
	EXTERN_C Int32 apm_send_dma_command(Ptr64 base_dma, APMPowerCmd cmd);

	/// @brief Send a APM command into it's own IO space.
	/// @param base_dma the IO base port.
	/// @param cmd the command.
	/// @return status code.
	EXTERN_C Int32 apm_send_io_command(UInt16 base_port, APMPowerCmd cmd);
} // namespace Kernel