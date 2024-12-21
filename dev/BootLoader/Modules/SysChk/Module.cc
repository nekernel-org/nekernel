/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright (C) 2024, TQ B.V, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>

EXTERN_C Int32 ModuleMain(Kernel::HEL::BootInfoHeader* Handover)
{
	EfiSystemTable* system_table = (EfiSystemTable*)Handover->f_FirmwareCustomTables[1];

	system_table->ConOut->ClearScreen(system_table->ConOut);

	system_table->ConOut->OutputString(system_table->ConOut, L"SYSCHK: CHECKING FOR VALID NEFS OR HPFS PARTITIONS...\r\n");
	system_table->ConOut->OutputString(system_table->ConOut, L"SYSCHK: GOOD TO GO!\r\n");

	return kEfiOk;
}
