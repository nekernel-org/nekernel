/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright (C) 2024, Theater Quality Inc, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>
#include <Modules/GfxMgr/FBMgr.h>
#include <Modules/GfxMgr/TextMgr.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NewKit/Macros.h>
#include <NewKit/Ref.h>
#include <BootKit/Thread.h>
#include <Modules/GfxMgr/FBMgr.h>

EXTERN_C Int32 ModuleMain(Kernel::HEL::BootInfoHeader* Handover)
{
	EfiSystemTable* system_table = (EfiSystemTable*)Handover->f_FirmwareCustomTables[1];

	EfiInputKey key{};

	system_table->ConIn->ReadKeyStroke(system_table->ConIn, &key);

	if (key.UnicodeChar == 'F' ||
		key.UnicodeChar == 'f')
	{
		UI::ui_draw_background();

		fb_init();

		FBDrawBitMapInRegion(zka_no_disk, ZKA_NO_DISK_HEIGHT, ZKA_NO_DISK_WIDTH, (kHandoverHeader->f_GOP.f_Width - ZKA_NO_DISK_WIDTH) / 2, (kHandoverHeader->f_GOP.f_Height - ZKA_NO_DISK_HEIGHT) / 2);

		fb_fini();
		
		return kEfiOk;
	}

	return kEfiFail;
}
