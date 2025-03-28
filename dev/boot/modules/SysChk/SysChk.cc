/*
 *	========================================================
 *
 *	SysChk
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>
#include <modules/CoreGfx/FBMgr.h>
#include <modules/CoreGfx/TextMgr.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NewKit/Macros.h>
#include <NewKit/Ref.h>
#include <BootKit/BootThread.h>
#include <modules/CoreGfx/FBMgr.h>

EXTERN_C Int32 ModuleMain(NeOS::HEL::BootInfoHeader* handover)
{
#ifdef __NE_AMD64__
	Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;

	if (partition_factory.IsPartitionValid())
		return kEfiOk;

	return kEfiFail;
#else
	return kEfiOk;
#endif
}
