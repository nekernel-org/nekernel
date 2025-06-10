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
STATIC EFI_GUID                   kGopGuid;

EXTERN_C Void rt_reset_hardware();

EXTERN EfiBootServices* BS;

/**
  @brief Finds and stores the GOP object.
*/
STATIC Bool boot_init_fb() noexcept {
  kGopGuid = EFI_GUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
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

  kHandoverHeader = new HEL::BootInfoHeader();

#ifdef ZBA_USE_FB
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
#endif  // ZBA_USE_FB

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
    kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
  } else {
    kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled = NO;
  }

  //-------------------------------------------------------------//
  // Allocate heap.
  //-------------------------------------------------------------//

  Boot::BootTextWriter writer;

  kHandoverHeader->f_BitMapStart = nullptr;           /* Start of bitmap. */
  kHandoverHeader->f_BitMapSize  = kHandoverBitMapSz; /* Size of bitmap in bytes. */
  Int32 trials                = 5 * 10000000;

  while (BS->AllocatePool(EfiLoaderData, kHandoverHeader->f_BitMapSize,
                          &kHandoverHeader->f_BitMapStart) != kEfiOk) {
    --trials;

    if (!trials) {
      writer.Write("BootZ: Unable to allocate sufficent memory, trying again with 2GB...\r");

      trials = 3 * 10000000;

      kHandoverHeader->f_BitMapSize = kHandoverBitMapSz / 2; /* Size of bitmap in bytes. */

      while (BS->AllocatePool(EfiLoaderData, kHandoverHeader->f_BitMapSize,
                              &kHandoverHeader->f_BitMapStart) != kEfiOk) {
        --trials;

        if (!trials) {
          writer.Write("BootZ: Unable to allocate sufficent memory, aborting...\r");
          Boot::Stop();
        }
      }
    }
  }

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

  Boot::BootFileReader reader_kernel(L"ne_kernel", image_handle);

  reader_kernel.ReadAll(0);

  // ------------------------------------------ //
  // If we succeed in reading the blob, then execute it.
  // ------------------------------------------ //

  if (reader_kernel.Blob()) {
    auto kernel_thread = Boot::BootThread(reader_kernel.Blob());
    kernel_thread.SetName("NeKernel");

    kHandoverHeader->f_KernelImage = reader_kernel.Blob();
    kHandoverHeader->f_KernelSz    = reader_kernel.Size();

    kernel_thread.Start(kHandoverHeader, YES);
  }

  CANT_REACH();
}
