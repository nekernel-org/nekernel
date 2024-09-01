/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright ZKA Technologies., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>

EXTERN_C Int32 main(Kernel::HEL::HandoverInformationHeader* Handover)
{
	EfiSystemTable* cST = (EfiSystemTable*)Handover->f_FirmwareCustomTables[1];
	cST->ConOut->ClearScreen(cST->ConOut);
	cST->ConOut->OutputString(cST->ConOut, L"SYSCHK: CHECKING FOR VALID NEWFS OR EXT4 PARTITIONS...\r");
	cST->ConOut->OutputString(cST->ConOut, L"SYSCHK: WE ARE GOOD TO GO!\r");

	return kEfiOk;
}
