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

/** Graphics related. */

STATIC EfiGraphicsOutputProtocol* kGop       = nullptr;
STATIC UInt16                     kGopStride = 0U;
STATIC EFI_GUID                   kGopGuid;

/** Related to jumping to the reset vector. */

EXTERN_C Void rt_reset_hardware();

EXTERN_C Kernel::VoidPtr boot_read_cr3();  // @brief Page directory inside cr3 register.

/**
  @brief Finds and stores the GOP object.
*/
STATIC Bool boot_init_fb() noexcept {
  kGopGuid = EFI_GUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  kGop     = nullptr;

  if (BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*) &kGop) != kEfiOk) return No;

  kGopStride = 4;

  return Yes;
}

EFI_GUID kEfiGlobalNamespaceVarGUID = {
    0x8BE4DF61, 0x93CA, 0x11D2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C}};

/// @brief BootloaderMain EFI entrypoint.
/// @param image_handle Handle of this image.
/// @param sys_table The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 BootloaderMain(EfiHandlePtr image_handle, EfiSystemTable* sys_table) {
  fw_init_efi(sys_table);  ///! Init the EFI library.

  ST->ConOut->ClearScreen(sys_table->ConOut);
  ST->ConOut->SetAttribute(sys_table->ConOut, kEFIYellow);

  ST->BootServices->SetWatchdogTimer(0, 0, 0, nullptr);
  ST->ConOut->EnableCursor(ST->ConOut, false);

  HEL::BootInfoHeader* handover_hdr = new HEL::BootInfoHeader();

  UInt32               map_key         = 0;
  UInt32               size_struct_ptr = sizeof(EfiMemoryDescriptor);
  EfiMemoryDescriptor* struct_ptr      = nullptr;
  UInt32               sz_desc         = sizeof(EfiMemoryDescriptor);
  UInt32               rev_desc        = 0;

  Boot::BootTextWriter writer;

  if (!boot_init_fb()) {
    writer.Write("BootZ: Invalid Framebuffer, can't boot to NeKernel.\r");
    Boot::Stop();
  }

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

  // ------------------------------------------- //
  // Grab MP services, extended to runtime.	   //
  // ------------------------------------------- //

  EFI_GUID               guid_mp = EFI_GUID(EFI_MP_SERVICES_PROTOCOL_GUID);
  EfiMpServicesProtocol* mp      = nullptr;

  BS->LocateProtocol(&guid_mp, nullptr, reinterpret_cast<VoidPtr*>(&mp));

  handover_hdr->f_HardwareTables.f_MpPtr = reinterpret_cast<VoidPtr>(mp);

  kHandoverHeader = handover_hdr;

  FB::fb_clear_video();

  UInt32 cnt_enabled  = 0;
  UInt32 cnt_disabled = 0;

  if (mp) {
    mp->GetNumberOfProcessors(mp, &cnt_disabled, &cnt_enabled);
    handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
  } else {
    handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = NO;
  }

  // Fill handover header now.

  handover_hdr->f_BitMapStart = nullptr;           /* Start of bitmap. */
  handover_hdr->f_BitMapSize  = kHandoverBitMapSz; /* Size of bitmap in bytes. */

  kHandoverHeader->f_BitMapStart = nullptr;           /* Start of bitmap. */
  kHandoverHeader->f_BitMapSize  = kHandoverBitMapSz; /* Size of bitmap in bytes. */

  UInt16 trials = 5;

  while (BS->AllocatePool(EfiLoaderData, kHandoverHeader->f_BitMapSize,
                          &kHandoverHeader->f_BitMapStart) != kEfiOk) {
    --trials;

    if (!trials) {
      writer.Write("BootZ: Unable to allocate sufficient memory, trying again with 2GB...\r");

      trials = 3;

      kHandoverHeader->f_BitMapSize = kHandoverBitMapSz / 2; /* Size of bitmap in bytes. */

      while (BS->AllocatePool(EfiLoaderData, kHandoverHeader->f_BitMapSize,
                              &kHandoverHeader->f_BitMapStart) != kEfiOk) {
        --trials;

        if (!trials) {
          writer.Write("BootZ: Unable to allocate sufficient memory, aborting...\r");
          Boot::Stop();
        }
      }
    }
  }

  handover_hdr->f_FirmwareCustomTables[Kernel::HEL::kHandoverTableBS] = (VoidPtr) BS;
  handover_hdr->f_FirmwareCustomTables[Kernel::HEL::kHandoverTableST] = (VoidPtr) ST;

  // ------------------------------------------ //
  // If we succeed in reading the blob, then execute it.
  // ------------------------------------------ //

  Boot::BootFileReader reader_syschk(L"chk.efi", image_handle);
  reader_syschk.ReadAll(0);

  Boot::BootThread* syschk_thread = nullptr;

  if (reader_syschk.Blob()) {
    syschk_thread = new Boot::BootThread(reader_syschk.Blob());
    syschk_thread->SetName("SysChk");

    syschk_thread->Start(handover_hdr, NO);
  }

  BS->GetMemoryMap(&size_struct_ptr, struct_ptr, &map_key, &sz_desc, &rev_desc);

  handover_hdr->f_FirmwareVendorLen = Boot::BStrLen(sys_table->FirmwareVendor);

  handover_hdr->f_Magic   = kHandoverMagic;
  handover_hdr->f_Version = kHandoverVersion;

  handover_hdr->f_HardwareTables.f_ImageKey    = map_key;
  handover_hdr->f_HardwareTables.f_ImageHandle = image_handle;

  // Provide fimware vendor name.

  Boot::BCopyMem(handover_hdr->f_FirmwareVendorName, sys_table->FirmwareVendor,
                 handover_hdr->f_FirmwareVendorLen);

  handover_hdr->f_FirmwareVendorLen = Boot::BStrLen(sys_table->FirmwareVendor);
  // Assign to global 'kHandoverHeader'.

  WideChar kernel_path[256U] = L"ne_kernel";
  UInt32   kernel_path_sz    = StrLen("ne_kernel");

  UInt32 sz_ver = sizeof(UInt64);
  UInt64 ver    = KERNEL_VERSION_BCD;

  ST->RuntimeServices->GetVariable(L"/props/kern_ver", kEfiGlobalNamespaceVarGUID, nullptr, &sz_ver,
                                   &ver);

  if (ver < KERNEL_VERSION_BCD) {
    ver = KERNEL_VERSION_BCD;

    ST->RuntimeServices->SetVariable(L"/props/kern_ver", kEfiGlobalNamespaceVarGUID, nullptr,
                                     &sz_ver, &ver);

    writer.Write("BootZ: Version has been updated: ").Write(ver).Write("\r");

    if (ST->RuntimeServices->GetVariable(L"/props/kernel_path", kEfiGlobalNamespaceVarGUID, nullptr,
                                         &kernel_path_sz, kernel_path) != kEfiOk) {
      /// access attributes (in order)
      /// EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS
      UInt32 attr = 0x00000001 | 0x00000002 | 0x00000004;

      ST->RuntimeServices->SetVariable(L"/props/kernel_path", kEfiGlobalNamespaceVarGUID, &attr,
                                       &kernel_path_sz, kernel_path);
    }
  } else {
    writer.Write("BootZ: Version: ").Write(ver).Write("\r");
  }

  // boot to kernel, if not bootnet this.

  Boot::BootFileReader reader_kernel(kernel_path, image_handle);
  reader_kernel.ReadAll(0);

  // ------------------------------------------ //
  // If we succeed in reading the blob, then execute it.
  // ------------------------------------------ //

  if (reader_kernel.Blob()) {
    handover_hdr->f_PageStart = boot_read_cr3();

    auto kernel_thread = Boot::BootThread(reader_kernel.Blob());

    kernel_thread.SetName("NeKernel");

    handover_hdr->f_KernelImage = reader_kernel.Blob();
    handover_hdr->f_KernelSz    = reader_kernel.Size();

    kernel_thread.Start(handover_hdr, YES);
  }

  Boot::BootFileReader reader_netboot(L"net.efi", image_handle);
  reader_netboot.ReadAll(0);

  if (!reader_netboot.Blob()) return kEfiFail;

  auto netboot_thread = Boot::BootThread(reader_netboot.Blob());
  netboot_thread.SetName("BootNet");

  return netboot_thread.Start(handover_hdr, NO);
}
