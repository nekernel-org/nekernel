/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#include <Modules/APM/APM.h>
#include <KernelKit/LPC.h>

using namespace Kernel;

/// @brief Send a APM command into it's own IO space.
/// @param base_dma the IO base port.
/// @param cmd the command.
/// @return status code.
EXTERN_C Int32 apm_send_io_command(UInt16 cmd, APMPowerCmd value)
{
	switch (cmd)
	{
	case kAPMPowerCommandReboot: {
		asm volatile(
			"ldr x0, =0x84000009\n"
			"hvc #0\n");

		return kErrorSuccess;
	}
	case kAPMPowerCommandShutdown: {
		asm volatile(
			"ldr x0, =0x84000008\n"
			"hvc #0\n");

		return kErrorSuccess;
	}
	default:
		return kErrorInvalidData;
	}
}
