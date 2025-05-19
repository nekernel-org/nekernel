/*
 *	========================================================
 *
 *	SysChk
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>
#include <BootKit/HW/SATA.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NewKit/Macros.h>
#include <NewKit/Ref.h>
#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/TextGfx.h>

// Makes the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "OS"
#endif  // !kMachineModel

EXTERN_C Int32 SysChkModuleMain(Kernel::HEL::BootInfoHeader* handover) {
  fw_init_efi((EfiSystemTable*) handover->f_FirmwareCustomTables[Kernel::HEL::kHandoverTableST]);

#if defined(__ATA_PIO__)
  Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;
#elif defined(__AHCI__)
  Boot::BDiskFormatFactory<BootDeviceSATA> partition_factory;
#endif
  if (partition_factory.IsPartitionValid()) return kEfiOk;

  return partition_factory.Format(kMachineModel);
}
