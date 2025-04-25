/*
 *	========================================================
 *
 *	SysChk
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>
#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/TextGfx.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NewKit/Macros.h>
#include <NewKit/Ref.h>
#include <BootKit/BootThread.h>
#include <modules/CoreGfx/CoreGfx.h>

// Makes the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "OS"
#endif // !kMachineModel

EXTERN_C Int32 SysChkModuleMain(Kernel::HEL::BootInfoHeader* handover)
{
#if defined(__ATA_PIO__)
	fw_init_efi((EfiSystemTable*)handover->f_FirmwareCustomTables[1]);

	Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;

	if (partition_factory.IsPartitionValid())
		return kEfiOk;

	return partition_factory.Format(kMachineModel) == YES;
#else
	NE_UNUSED(handover);

	return kEfiOk;
#endif
}
