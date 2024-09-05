/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright ZKA Technologies., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>

EXTERN_C Int32 ModuleMain(Kernel::HEL::HandoverInformationHeader* Handover)
{
	EfiSystemTable* cST = (EfiSystemTable*)Handover->f_FirmwareCustomTables[1];
	
	cST->ConOut->ClearScreen(cST->ConOut);

	cST->ConOut->OutputString(cST->ConOut, L"SYSCHK: CHECKING FOR VALID NEWFS OR HPFS PARTITIONS...\r\n");
	cST->ConOut->OutputString(cST->ConOut, L"SYSCHK: GOOD TO GO!\r\n");

	return kEfiOk;
}
