/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <CFKit/Property.h>
#include <FirmwareKit/Handover.h>
#include <HALKit/ARM64/ApplicationProcessor.h>
#include <HALKit/ARM64/Processor.h>
#include <KernelKit/CodeMgr.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/MemoryMgr.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <NetworkKit/IPC.h>
#include <NewKit/Json.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <modules/CoreGfx/CoreGfx.h>

#ifndef __NE_MODULAR_KERNEL_COMPONENTS__
EXTERN_C void hal_init_platform(Kernel::HEL::BootInfoHeader* handover_hdr) {
  using namespace Kernel;

  /************************************************** */
  /*     INITIALIZE AND VALIDATE HEADER.              */
  /************************************************** */

  kHandoverHeader = handover_hdr;

  if (kHandoverHeader->f_Magic != kHandoverMagic &&
      kHandoverHeader->f_Version != kHandoverVersion) {
    return;
  }

  FB::fb_clear_video();

#ifdef __NE_ARM64_EFI__
  fw_init_efi((EfiSystemTable*) handover_hdr->f_FirmwareCustomTables[1]);

  Boot::ExitBootServices(handover_hdr->f_HardwareTables.f_ImageKey,
                         handover_hdr->f_HardwareTables.f_ImageHandle);
#endif

  /************************************** */
  /*     INITIALIZE BIT MAP.              */
  /************************************** */

  kKernelBitMpSize  = kHandoverHeader->f_BitMapSize;
  kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
      reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

  /// @note do initialize the interrupts after it.

  for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Capacity(); ++index) {
    HardwareThreadScheduler::The()[index].Leak()->Kind() = ThreadKind::kAPStandard;
    HardwareThreadScheduler::The()[index].Leak()->Busy(NO);
  }

  for (SizeT index = 0UL; index < UserProcessScheduler::The().TheCurrentTeam().AsArray().Count();
       ++index) {
    UserProcessScheduler::The().TheCurrentTeam().AsArray()[index].Status =
        ProcessStatusKind::kInvalid;
  }

  rtl_create_user_process(sched_idle_task, "MgmtSrv");    //! Mgmt command server.
  rtl_create_user_process(sched_idle_task, "LaunchSrv");  //! launchd
  rtl_create_user_process(sched_idle_task, "SecSrv");     //! Login Server

  Kernel::mp_init_cores();

  while (YES)
    ;
}
#endif