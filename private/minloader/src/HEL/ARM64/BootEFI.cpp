// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NeKit/Macros.h>
#include <NeKit/Ref.h>
#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/TextGfx.h>

/** Graphics related. */

EXTERN_C Void rt_reset_hardware();

EXTERN EfiBootServices* BS;

/**
  @brief Finds and stores the GOP object.
*/
#ifdef BOOTZ_USE_FB
STATIC EfiGraphicsOutputProtocol* kGop       = nullptr;
STATIC UInt16                     kGopStride = 0U;
STATIC EFI_GUID                   kGopGuid;

STATIC Bool boot_init_fb(Void) {
  kGopGuid = EFI_GUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  kGop     = nullptr;

  if (BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*) &kGop) != kEfiOk) return No;

  kGopStride = 4;

  return Yes;
}
#endif

EFI_GUID kEfiGlobalNamespaceVarGUID = {
    0x8BE4DF61, 0x93CA, 0x11D2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C}};

EXTERN EfiBootServices* BS;

/// @brief BootloaderMain EFI entrypoint.
/// @param image_handle Handle of this image.
/// @param sys_table The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 BootloaderMain(EfiHandlePtr image_handle, EfiSystemTable* sys_table) {
  if (!image_handle || !sys_table) return kEfiFail;

  fw_init_efi(sys_table);  ///! Init the EFI library.

  kHandoverHeader = new HEL::BootInfoHeader();

#ifdef BOOTZ_USE_FB
  if (!boot_init_fb()) return kEfiFail;  ///! Init the GOP.

  for (SizeT index_vt = 0; index_vt < sys_table->NumberOfTableEntries; ++index_vt) {
    Char* vendor_table =
        reinterpret_cast<Char*>(sys_table->ConfigurationTable[index_vt].VendorTable);

    // ACPI's 'RSD PTR', which contains the ACPI SDT (MADT, FACP...)
    if (vendor_table[0] == 'R' && vendor_table[1] == 'S' && vendor_table[2] == 'D' &&
        vendor_table[3] == ' ' && vendor_table[4] == 'P' && vendor_table[5] == 'T' &&
        vendor_table[6] == 'R' && vendor_table[7] == ' ') {
      kHandoverHeader->f_HardwareTables.f_VendorPtr = (VoidPtr) vendor_table;
      break;
    }
  }

  // ------------------------------------------ //
  // draw background color.
  // ------------------------------------------ //

  kHandoverHeader->f_GOP.f_The          = kGop->Mode->FrameBufferBase;
  kHandoverHeader->f_GOP.f_Width        = kGop->Mode->Info->VerticalResolution;
  kHandoverHeader->f_GOP.f_Height       = kGop->Mode->Info->HorizontalResolution;
  kHandoverHeader->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
  kHandoverHeader->f_GOP.f_PixelFormat  = kGop->Mode->Info->PixelFormat;
  kHandoverHeader->f_GOP.f_Size         = kGop->Mode->FrameBufferSize;
#endif  // BOOTZ_USE_FB

  // ------------------------------------------- //
  // Grab MP services, extended to runtime.	   //
  // ------------------------------------------- //

  EFI_GUID               guid_mp = EFI_GUID(EFI_MP_SERVICES_PROTOCOL_GUID);
  EfiMpServicesProtocol* mp      = nullptr;

  BS->LocateProtocol(&guid_mp, nullptr, reinterpret_cast<VoidPtr*>(&mp));
  kHandoverHeader->f_HardwareTables.f_MpPtr = reinterpret_cast<VoidPtr>(mp);

  UInt32 cnt_enabled  = 0;
  UInt32 cnt_disabled = 0;

  if (mp) {
    mp->GetNumberOfProcessors(mp, &cnt_disabled, &cnt_enabled);
    kHandoverHeader->f_NumberOfProcessors                      = cnt_enabled;
    kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
  } else {
    kHandoverHeader->f_NumberOfProcessors                      = 1;
    kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled = NO;
  }

  //-------------------------------------------------------------//
  // TODO: Allocate heap.
  //-------------------------------------------------------------//

  // ------------------------------------------ //
  // null these fields, to avoid being reused later.
  // ------------------------------------------ //

  kHandoverHeader->f_FirmwareCustomTables[0] = nullptr;
  kHandoverHeader->f_FirmwareCustomTables[1] = nullptr;

  kHandoverHeader->f_FirmwareVendorLen = Boot::BStrLen(sys_table->FirmwareVendor);

  kHandoverHeader->f_Magic   = kHandoverMagic;
  kHandoverHeader->f_Version = kHandoverVersion;

  // Provide fimware vendor name.

  Boot::BCopyMem(kHandoverHeader->f_FirmwareVendorName, sys_table->FirmwareVendor,
                 kHandoverHeader->f_FirmwareVendorLen);

  kHandoverHeader->f_FirmwareVendorLen = Boot::BStrLen(sys_table->FirmwareVendor);

  UIntPtr sz_recover_mode = sizeof(Bool);
  Bool    recover_mode    = 0;

  ST->RuntimeServices->GetVariable(L"/props/recover_mode", kEfiGlobalNamespaceVarGUID, nullptr,
                                   &sz_recover_mode, &recover_mode);

  kHandoverHeader->f_RecoverMode = recover_mode;

  /* one shot flag, attributes 0 delete it. */
  ST->RuntimeServices->SetVariable(L"/props/recover_mode", kEfiGlobalNamespaceVarGUID, 0, 0,
                                   nullptr);

  Boot::BootFileReader reader_hal(L"hal.arm64.dll", image_handle);
  reader_hal.ReadAll(0);

  if (reader_hal.Blob()) {
    kHandoverHeader->f_HALImage = reader_hal.Blob();
    kHandoverHeader->f_HALSz = reader_hal.Size();
  }

  Boot::BootFileReader reader_kernel(L"vmoskrnl.exe", image_handle);

  reader_kernel.ReadAll(0);

  // ------------------------------------------ //
  // If we succeed in reading the blob, then execute it.
  // ------------------------------------------ //

  if (reader_kernel.Blob()) {
    auto kernel_thread = Boot::BootThread(reader_kernel.Blob());
    kernel_thread.SetName("Ne.app NeKernel");

    kHandoverHeader->f_KernelImage = reader_kernel.Blob();
    kHandoverHeader->f_KernelSz    = reader_kernel.Size();

    kernel_thread.Start(kHandoverHeader, YES);
  }

  CANT_REACH();
}
