/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <CFKit/Property.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/EFI/EFI.h>
#include <KernelKit/CodeMgr.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/Timer.h>
#include <NetworkKit/IPC.h>
#include <StorageKit/AHCI.h>
#include <misc/BenchKit/X64Chrono.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <modules/CoreGfx/TextGfx.h>

#ifndef __NE_MODULAR_KERNEL_COMPONENTS__
EXTERN_C Kernel::VoidPtr kInterruptVectorTable[];

/// @brief Kernel init function.
/// @param handover_hdr Handover boot header.
EXTERN_C Int32 hal_init_platform(Kernel::HEL::BootInfoHeader* handover_hdr) {
  using namespace Kernel;

  if (handover_hdr->f_Magic != kHandoverMagic && handover_hdr->f_Version != kHandoverVersion) {
    return kEfiFail;
  }

  HAL::rt_sti();

  kHandoverHeader = handover_hdr;

  FB::fb_clear_video();

  fw_init_efi((EfiSystemTable*) handover_hdr->f_FirmwareCustomTables[1]);

  Boot::ExitBootServices(handover_hdr->f_HardwareTables.f_ImageKey,
                         handover_hdr->f_HardwareTables.f_ImageHandle);

  kKernelVM = kHandoverHeader->f_PageStart;

  if (!kKernelVM) {
    MUST_PASS(kKernelVM);
    return kEfiFail;
  }

  hal_write_cr3(kKernelVM);

  /************************************** */
  /*     INITIALIZE BIT MAP.              */
  /************************************** */

  kKernelBitMpSize = kHandoverHeader->f_BitMapSize;
  kKernelBitMpStart =
      reinterpret_cast<VoidPtr>(reinterpret_cast<UIntPtr>(kHandoverHeader->f_BitMapStart));

  /************************************** */
  /*     INITIALIZE GDT AND SEGMENTS. */
  /************************************** */

  STATIC CONST auto kGDTEntriesCount = 8;

  STATIC HAL::Detail::NE_TSS kKernelTSS{};

  kKernelTSS.fRsp0 = (UInt64) kHandoverHeader->f_StackTop;
  kKernelTSS.fIopb = sizeof(HAL::Detail::NE_TSS);

  /* The GDT, mostly descriptors for user and kernel segments. */
  STATIC HAL::Detail::NE_GDT_ENTRY ALIGN(0x08) kGDTArray[kGDTEntriesCount] = {
      {.fLimitLow   = 0,
       .fBaseLow    = 0,
       .fBaseMid    = 0,
       .fAccessByte = 0x00,
       .fFlags      = 0x00,
       .fBaseHigh   = 0},  // Null entry
      {.fLimitLow   = 0x0,
       .fBaseLow    = 0,
       .fBaseMid    = 0,
       .fAccessByte = 0x9A,
       .fFlags      = 0xAF,
       .fBaseHigh   = 0},  // Kernel code
      {.fLimitLow   = 0x0,
       .fBaseLow    = 0,
       .fBaseMid    = 0,
       .fAccessByte = 0x92,
       .fFlags      = 0xCF,
       .fBaseHigh   = 0},  // Kernel data
      {},                // TSS data low
      {},                // TSS data high
      {.fLimitLow   = 0x0,
       .fBaseLow    = 0,
       .fBaseMid    = 0,
       .fAccessByte = 0xFA,
       .fFlags      = 0xAF,
       .fBaseHigh   = 0},  // User code
      {.fLimitLow   = 0x0,
       .fBaseLow    = 0,
       .fBaseMid    = 0,
       .fAccessByte = 0xF2,
       .fFlags      = 0xCF,
       .fBaseHigh   = 0},  // User data
  };

  kGDTArray[3].fLimitLow   = sizeof(HAL::Detail::NE_TSS) - 1;
  kGDTArray[3].fBaseLow    = ((UIntPtr) &kKernelTSS) & 0xFFFF;
  kGDTArray[3].fBaseMid    = (((UIntPtr) &kKernelTSS) >> 16) & 0xFF;
  kGDTArray[3].fAccessByte = 0x89;  // Present, type 9 = 64-bit available TSS
  kGDTArray[3].fFlags      = 0x20 | ((((UIntPtr) &kKernelTSS) >> 24) & 0x0F);
  kGDTArray[3].fBaseHigh   = (((UIntPtr) &kKernelTSS) >> 24) & 0xFF;

  kGDTArray[4].fLimitLow   = ((UIntPtr) &kKernelTSS >> 32) & 0xFFFF;
  kGDTArray[4].fBaseLow    = 0;
  kGDTArray[4].fBaseMid    = 0;
  kGDTArray[4].fAccessByte = 0;
  kGDTArray[4].fFlags      = 0;
  kGDTArray[4].fBaseHigh   = 0;

  // Load memory descriptors.
  HAL::Register64 gdt_reg;

  gdt_reg.Base  = reinterpret_cast<UIntPtr>(kGDTArray);
  gdt_reg.Limit = (sizeof(HAL::Detail::NE_GDT_ENTRY) * kGDTEntriesCount) - 1;

  //! GDT will load hal_read_init after it successfully loads the segments.
  HAL::GDTLoader gdt_loader;
  gdt_loader.Load(gdt_reg);

  return kEfiFail;
}

EXTERN_C Kernel::Void hal_real_init(Kernel::Void) noexcept {
  using namespace Kernel;

  for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Capacity(); ++index) {
    HardwareThreadScheduler::The()[index].Leak()->Kind() = ThreadKind::kAPStandard;
    HardwareThreadScheduler::The()[index].Leak()->ID()   = index;
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

  HAL::mp_init_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

#ifdef __FSKIT_INCLUDES_HEFS__
  HeFS::fs_init_hefs();
#endif

#ifdef __FSKIT_INCLUDES_NEFS__
  NeFS::fs_init_nefs();
#endif

  HAL::Register64 idt_reg;
  idt_reg.Base = reinterpret_cast<UIntPtr>(kInterruptVectorTable);

  HAL::IDTLoader idt_loader;

  idt_loader.Load(idt_reg);

  while (YES)
    ;
}
#endif  // ifndef __NE_MODULAR_KERNEL_COMPONENTS__
