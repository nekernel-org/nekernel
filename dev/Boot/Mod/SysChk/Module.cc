/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright (C) 2024, Theater Quality Corp, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>
#include <Mod/GfxMgr/FBMgr.h>
#include <Mod/GfxMgr/TextMgr.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NewKit/Macros.h>
#include <NewKit/Ref.h>
#include <BootKit/Thread.h>
#include <Mod/GfxMgr/FBMgr.h>

EXTERN_C Int32 ModuleMain(Kernel::HEL::BootInfoHeader* handover)
{
	if (!handover)
		return kEfiFail;

	Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;

	return !partition_factory.IsPartitionValid() ? kEfiOk : kEfiFail;
}
