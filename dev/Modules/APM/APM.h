/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Kernel
{
	typedef Int32 APMPowerCmd;

	enum
	{
		kAPMPowerCommandInvalid	 = 0x00,
		kAPMPowerCommandStop	 = 0x01,
		kAPMPowerCommandStart	 = 0x02,
		kAPMPowerCommandSleep	 = 0x04,
		kAPMPowerCommandWakeup	 = 0x06,
		kAPMPowerCommandShutdown = 0x07,
		kAPMPowerCommandReboot	 = 0x08,
	};

	/// @brief Send a APM command into it's own DMA space.
	/// @param base_dma the DMA base address.
	/// @param cmd the command.
	/// @return status code.
	EXTERN_C Int32 apm_send_dma_command(Ptr64 register_addr, APMPowerCmd value);

	/// @brief Send a APM command into it's own IO space.
	/// @param base_dma the IO base port.
	/// @param cmd the command.
	/// @return status code.
	EXTERN_C Int32 apm_send_io_command(UInt16 cmd, APMPowerCmd value);
} // namespace Kernel
