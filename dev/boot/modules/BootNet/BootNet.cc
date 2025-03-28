/*
 *	========================================================
 *
 *	BootNet
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <modules/BootNet/BootNet.h>
#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>

EXTERN_C Int32 ModuleMain(NeOS::HEL::BootInfoHeader* handover)
{
	NETBOOT_INTERNET_HEADER inet{};

	memset(&inet, 0, sizeof(NETBOOT_INTERNET_HEADER));

	/// TODO: Read packet from JSON file 'bootnet.json'

	if (inet.PatchLength < 1)
	{
		Boot::BootTextWriter writer;
		writer.Write("NetBootLauncher: No executable attached to the packet, aborting.\r");

		return kEfiFail;
	}

	if (!inet.EEPROM)
	{
		Boot::BootThread thread(inet.PatchData);

		if (thread.IsValid())
			return thread.Start(handover, YES);

		return kEfiFail;
	}
	else
	{
		Boot::BootTextWriter writer;
		writer.Write("NetBootLauncher: EEPROM flash is not available as of right now.\r");

		/// TODO: Program new firmware to EEPROM (if crc and size matches)

		return kEfiFail; // TODO: Add support for EEPROM firmware update.
	}

	return kEfiFail;
}
