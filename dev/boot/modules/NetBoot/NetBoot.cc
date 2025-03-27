/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <modules/NetBoot/NetBoot.h>
#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>

EXTERN_C Int32 ModuleMain(NeOS::HEL::BootInfoHeader* handover)
{
	NETBOOT_INTERNET_HEADER inet{};

	/// TODO: Read packet from JSON file 'netboot.json'

	if (inet.PatchLength < 0)
	{
		Boot::BootTextWriter writer;
		writer.Write("NetBootLauncher: No Patch attached to packet.\r");

		return kEfiFail;
	}

	if (!inet.EEPROM)
	{
		Boot::BootThread thread(inet.PatchData);

		if (thread.IsValid())
			return thread.Start(handover, YES);
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
