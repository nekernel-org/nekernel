/*
 *	========================================================
 *
 *	BootNet
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <modules/BootNet/BootNet.h>
#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>

EXTERN_C Int32 ModuleMain(Kernel::HEL::BootInfoHeader* handover)
{
	BOOTNET_INTERNET_HEADER inet{};

	memset(&inet, 0, sizeof(BOOTNET_INTERNET_HEADER));

	/// TODO: Read packet from JSON file 'bootnet.json'

	if (inet.Length < 1)
	{
		Boot::BootTextWriter writer;
		writer.Write("BootNetLauncher: No executable attached to the packet, aborting.\r");

		return kEfiFail;
	}

	if (!inet.ImpliesEEPROM)
	{
		Boot::BootThread thread(inet.Data);

		if (thread.IsValid())
			return thread.Start(handover, YES);

		return kEfiFail;
	}
	else
	{
		Boot::BootTextWriter writer;
		writer.Write("BootNetLauncher: EEPROM flash is not available as of right now.\r");

		/// TODO: Program new firmware to EEPROM (if crc and size matches)

		return kEfiFail; // TODO: Add support for EEPROM firmware update.
	}

	return kEfiFail;
}
