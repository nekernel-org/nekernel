/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

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

#ifndef kExpectedWidth
#define kExpectedWidth (800)
#endif

#ifndef kExpectedHeight
#define kExpectedHeight (600)
#endif

/** Graphics related. */

STATIC EfiGraphicsOutputProtocol* kGop       = nullptr;
STATIC UInt16                     kGopStride = 0U;
STATIC EfiGUID                    kGopGuid;

EXTERN_C Void rt_reset_hardware();

EXTERN EfiBootServices* BS;

/**
  @brief Finds and stores the GOP object.
*/
STATIC Bool boot_init_fb() noexcept {
  kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  kGop     = nullptr;

  if (BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*) &kGop) != kEfiOk) return No;

  kGopStride = 4;

  for (SizeT i = 0; i < kGop->Mode->MaxMode; ++i) {
    EfiGraphicsOutputProtocolModeInformation* infoPtr = nullptr;
    UInt32                                    sz      = 0U;

    kGop->QueryMode(kGop, i, &sz, &infoPtr);

    if (infoPtr->HorizontalResolution == kExpectedWidth &&
        infoPtr->VerticalResolution == kExpectedHeight) {
      kGop->SetMode(kGop, i);
      return Yes;
    }
  }

  return No;
}

EXTERN EfiBootServices* BS;

/// @brief BootloaderMain EFI entrypoint.
/// @param image_handle Handle of this image.
/// @param sys_table The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 BootloaderMain(EfiHandlePtr image_handle, EfiSystemTable* sys_table) {
  fw_init_efi(sys_table);  ///! Init the EFI library.

  HEL::BootInfoHeader* handover_hdr = new HEL::BootInfoHeader();

  UInt32 map_key = 0;

#ifdef ZBA_USE_FB
  if (!boot_init_fb()) return 1;  ///! Init the GOP.

  for (SizeT index_vt = 0; index_vt < sys_table->NumberOfTableEntries; ++index_vt) {
    Char* vendor_table =
        reinterpret_cast<Char*>(sys_table->ConfigurationTable[index_vt].VendorTable);

    // ACPI's 'RSD PTR', which contains the ACPI SDT (MADT, FACP...)
    if (vendor_table[0] == 'R' && vendor_table[1] == 'S' && vendor_table[2] == 'D' &&
        vendor_table[3] == ' ' && vendor_table[4] == 'P' && vendor_table[5] == 'T' &&
        vendor_table[6] == 'R' && vendor_table[7] == ' ') {
      handover_hdr->f_HardwareTables.f_VendorPtr = (VoidPtr) vendor_table;
      break;
    }
  }

  // ------------------------------------------ //
  // draw background color.
  // ------------------------------------------ //

  handover_hdr->f_GOP.f_The          = kGop->Mode->FrameBufferBase;
  handover_hdr->f_GOP.f_Width        = kGop->Mode->Info->VerticalResolution;
  handover_hdr->f_GOP.f_Height       = kGop->Mode->Info->HorizontalResolution;
  handover_hdr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
  handover_hdr->f_GOP.f_PixelFormat  = kGop->Mode->Info->PixelFormat;
  handover_hdr->f_GOP.f_Size         = kGop->Mode->FrameBufferSize;
#endif  // ZBA_USE_FB

  // ------------------------------------------- //
  // Grab MP services, extended to runtime.	   //
  // ------------------------------------------- //

  EfiGUID                guid_mp = EfiGUID(EFI_MP_SERVICES_PROTOCOL_GUID);
  EfiMpServicesProtocol* mp      = nullptr;

  BS->LocateProtocol(&guid_mp, nullptr, reinterpret_cast<VoidPtr*>(&mp));
  handover_hdr->f_HardwareTables.f_MpPtr = reinterpret_cast<VoidPtr>(mp);

  // Assign to global 'kHandoverHeader'.
  kHandoverHeader = handover_hdr;

  fb_init();

  FB::fb_clear_video();

  FBDrawBitMapInRegion(zka_disk, NE_DISK_WIDTH, NE_DISK_HEIGHT,
                       (kHandoverHeader->f_GOP.f_Width - NE_DISK_WIDTH) / 2,
                       (kHandoverHeader->f_GOP.f_Height - NE_DISK_HEIGHT) / 2);

  fb_clear();

  UInt32 cnt_enabled  = 0;
  UInt32 cnt_disabled = 0;

  if (mp) {
    mp->GetNumberOfProcessors(mp, &cnt_disabled, &cnt_enabled);
    handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
  } else {
    handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = NO;
  }

  //-------------------------------------------------------------//
  // Allocate heap.
  //-------------------------------------------------------------//

  Boot::BootTextWriter writer;

  handover_hdr->f_BitMapStart = nullptr;           /* Start of bitmap. */
  handover_hdr->f_BitMapSize  = kHandoverBitMapSz; /* Size of bitmap in bytes. */
  Int32 trials                = 5 * 10000000;

  while (BS->AllocatePool(EfiLoaderData, handover_hdr->f_BitMapSize,
                          &handover_hdr->f_BitMapStart) != kEfiOk) {
    --trials;

    if (!trials) {
      writer.Write("BootZ: Unable to allocate sufficent memory, trying again with 2GB...\r");

      trials = 3 * 10000000;

      handover_hdr->f_BitMapSize = kHandoverBitMapSz / 2; /* Size of bitmap in bytes. */

      while (BS->AllocatePool(EfiLoaderData, handover_hdr->f_BitMapSize,
                              &handover_hdr->f_BitMapStart) != kEfiOk) {
        --trials;

        if (!trials) {
          writer.Write("BootZ: Unable to allocate sufficent memory, aborting...\r");
          Boot::Stop();
        }
      }
    }
  }

  Boot::BootFileReader reader_syschk(L"chk.efi", image_handle);
  reader_syschk.ReadAll(0);

  Boot::BootThread* syschk_thread = nullptr;

  if (reader_syschk.Blob()) {
    syschk_thread = new Boot::BootThread(reader_syschk.Blob());
    syschk_thread->SetName("BootZ: System Check");

    if (syschk_thread->Start(handover_hdr, NO) != kEfiOk) {
      fb_init();

      FB::fb_clear_video();

      FBDrawBitMapInRegion(zka_no_disk, NE_NO_DISK_WIDTH, NE_NO_DISK_HEIGHT,
                           (kHandoverHeader->f_GOP.f_Width - NE_NO_DISK_WIDTH) / 2,
                           (kHandoverHeader->f_GOP.f_Height - NE_NO_DISK_HEIGHT) / 2);

      fb_clear();
    }
  }

  // ------------------------------------------ //
  // null these fields, to avoid being reused later.
  // ------------------------------------------ //

  handover_hdr->f_FirmwareCustomTables[0] = nullptr;
  handover_hdr->f_FirmwareCustomTables[1] = nullptr;

  handover_hdr->f_FirmwareVendorLen = Boot::BStrLen(sys_table->FirmwareVendor);

  handover_hdr->f_Magic   = kHandoverMagic;
  handover_hdr->f_Version = kHandoverVersion;

  // Provide fimware vendor name.

  Boot::BCopyMem(handover_hdr->f_FirmwareVendorName, sys_table->FirmwareVendor,
                 handover_hdr->f_FirmwareVendorLen);

  handover_hdr->f_FirmwareVendorLen = Boot::BStrLen(sys_table->FirmwareVendor);

  Boot::BootFileReader reader_kernel(L"krnl.efi", image_handle);

  reader_kernel.ReadAll(0);

  // ------------------------------------------ //
  // If we succeed in reading the blob, then execute it.
  // ------------------------------------------ //

  if (reader_kernel.Blob()) {
    auto kernel_thread = Boot::BootThread(reader_kernel.Blob());
    kernel_thread.SetName("BootZ: Kernel.");

    handover_hdr->f_KernelImage = reader_kernel.Blob();
    handover_hdr->f_KernelSz    = reader_kernel.Size();

    Boot::ExitBootServices(map_key, image_handle);

    kernel_thread.Start(handover_hdr, YES);
  }

  CANT_REACH();
}
