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
#define kMachineModel "NeKrnl"
#endif // !kMachineModel

EXTERN_C Int32 SysChkModuleMain(Kernel::HEL::BootInfoHeader* handover)
{
#ifdef __NE_AMD64__
	Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;

	if (partition_factory.IsPartitionValid())
		return kEfiOk;

	Boot::BDiskFormatFactory<BootDeviceATA>::BFileDescriptor desc{};
	
	desc.fFileName[0] = '/';
	desc.fFileName[1] = 0;
	desc.fKind = kNeFSCatalogKindDir;

	partition_factory.Format(kMachineModel, &desc, sizeof(Boot::BDiskFormatFactory<BootDeviceATA>::BFileDescriptor));

	if (partition_factory.IsPartitionValid())
		return kEfiOk;

	return kEfiFail;
#else
	return kEfiOk;
#endif
}
