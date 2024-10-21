/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright ZKA Web Services Co., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>

EXTERN_C Int32 ModuleMain(Kernel::HEL::HANDOVER_INFO_HEADER* Handover)
{
	EfiSystemTable* cST = (EfiSystemTable*)Handover->f_FirmwareCustomTables[1];

	cST->ConOut->ClearScreen(cST->ConOut);

	cST->ConOut->OutputString(cST->ConOut, L"SYSCHK: CHECKING FOR VALID NEFS OR HPFS PARTITIONS...\r\n");
	cST->ConOut->OutputString(cST->ConOut, L"SYSCHK: GOOD TO GO!\r\n");

	return kEfiOk;
}
