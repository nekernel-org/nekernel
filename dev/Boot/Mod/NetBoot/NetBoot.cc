/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <Mod/NetBoot/NetBoot.h>
#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>

EXTERN_C Int32 ModuleMain(NeOS::HEL::BootInfoHeader* handover)
{
	NETBOOT_INTERNET_HEADER inet{};

	/// TODO: Read Packet from localhost

	if (inet.PatchLength < 0)
	{
		Boot::BootTextWriter writer;
		writer.Write("NetBootLauncher: No Patch attached to packet.\r");

		return kEfiFail;
	}
	else if (inet.Preflight)
	{
		Boot::BootTextWriter writer;
		writer.Write("NetBootLauncher: Preflight over preflight response.\r");

		return kEfiFail;
	}
	else if (inet.EEPROM)
	{
		Boot::BootTextWriter writer;
		writer.Write("NetBootLauncher: EEPROM flash not available for now.\r");

		return kEfiFail;
	}

	Boot::BootThread thread(inet.Data);

	if (thread.IsValid())
		return thread.Start(handover, YES);

	return kEfiFail;
}
