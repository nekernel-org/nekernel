/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hxx>
#include <NewKit/Ref.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#else
#error This CPU is unknown.
#endif  // ifdef __x86_64__

#define kMaxBufSize 256

/// @brief Bootloader main type.
typedef void (*bt_main_type)(HEL::HandoverInformationHeader* handoverInfo);

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return
EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);  // Init the EFI library.
  InitGOP();             // Init the GOP.

  BTextWriter writer;
  /// Splash screen stuff

  writer.Write(L"Mahrouss-Logic (R) NewOS: ").Write(BVersionString::Shared());

  writer.Write(L"\r\nNewOS: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r\n");

  BootDeviceATA ataDev;
  Boolean isEpmFound = No;

  /// if ATA drive is initialized and EFI vendor supports an EPM scheme.
  /// @EDK tells our OS that it supports EPM scheme as well.
  if (ataDev && SystemTable->FirmwareVendor[0] == '@') {
    Char namePart[kEPMNameLength] = {"NewBoot"};
    /// tries to read an EPM block, or writes one if it fails.
    isEpmFound = boot_write_epm_partition(namePart, kEPMNameLength, &ataDev);
  } else {
    writer.Write(L"NewOS: This computer can't work with NewOS.\r\n");
    return kEfiFail;
  }

  /// Read Kernel blob.

  BFileReader kernelImg(L".NEWDEV", ImageHandle);

  kernelImg.Size(kMaxBufSize);
  kernelImg.ReadAll();

  if (kernelImg.Error() == BFileReader::kOperationOkay) {
    UInt32 MapKey = 0;
    UInt32* SizePtr = nullptr;
    EfiMemoryDescriptor* Descriptor = nullptr;
    UInt32 SzDesc = 0;
    UInt32 RevDesc = 0;

    if (BS->AllocatePool(EfiLoaderData, sizeof(UInt32), (VoidPtr*)&SizePtr) !=
        kEfiOk) {
      EFI::RaiseHardError(L"__bad_alloc", L"NewBoot ran out of memory!");
    }

    /****
     *
     *  Load kernel into memory.
     *
     */

    *SizePtr = sizeof(EfiMemoryDescriptor);

    if (BS->AllocatePool(EfiLoaderData, sizeof(EfiMemoryDescriptor),
                         (VoidPtr*)&Descriptor) != kEfiOk) {
      EFI::RaiseHardError(L"__bad_alloc", L"NewBoot ran out of memory!");
    }

    /****
     *
     *  Get machine memory map.
     *
     */

    while (BS->GetMemoryMap(SizePtr, Descriptor, &MapKey, &SzDesc, &RevDesc) !=
           kEfiOk)
      ;

    HEL::HandoverInformationHeader* handoverHdrPtr = nullptr;

    BS->AllocatePool(EfiLoaderData, sizeof(HEL::HandoverInformationHeader),
                     (VoidPtr*)&handoverHdrPtr);

    handoverHdrPtr->f_GOP.f_The = kGop->Mode->FrameBufferBase;
    handoverHdrPtr->f_GOP.f_Width = kGop->Mode->Info->VerticalResolution;
    handoverHdrPtr->f_GOP.f_Height = kGop->Mode->Info->HorizontalResolution;
    handoverHdrPtr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
    handoverHdrPtr->f_GOP.f_PixelFormat = kGop->Mode->Info->PixelFormat;
    handoverHdrPtr->f_GOP.f_Size = kGop->Mode->FrameBufferSize;

    handoverHdrPtr->f_PhysicalStart =
        reinterpret_cast<voidPtr>(Descriptor->PhysicalStart);
    handoverHdrPtr->f_PhysicalSize = Descriptor->NumberOfPages;

    handoverHdrPtr->f_VirtualStart =
        reinterpret_cast<voidPtr>(Descriptor->VirtualStart);

    handoverHdrPtr->f_VirtualSize = Descriptor->NumberOfPages; /* # of pages */

    handoverHdrPtr->f_FirmwareVendorLen = BStrLen(SystemTable->FirmwareVendor);

    BCopyMem(handoverHdrPtr->f_FirmwareVendorName, SystemTable->FirmwareVendor,
             handoverHdrPtr->f_FirmwareVendorLen);

#ifdef __DEBUG__
    writer.Write(L"NewOS: Fetching ACPI's 'RSD PTR'...").Write(L"\r\n");
#endif

    for (SizeT indexVT = 0; indexVT < SystemTable->NumberOfTableEntries;
         ++indexVT) {
      volatile Char* vendorTable = reinterpret_cast<volatile Char*>(
          SystemTable->ConfigurationTable[indexVT].VendorTable);

      if (vendorTable[0] == 'R' && vendorTable[1] == 'S' &&
          vendorTable[2] == 'D' && vendorTable[3] == ' ' &&
          vendorTable[4] == 'P' && vendorTable[5] == 'T' &&
          vendorTable[6] == 'R' && vendorTable[7] == ' ') {
        handoverHdrPtr->f_HardwareTables.f_RsdPtr = (VoidPtr)vendorTable;

#ifdef __DEBUG__
        writer.Write(L"NewOS: Found ACPI's 'RSD PTR' table on this machine.")
            .Write(L"\r\n");
#endif

        break;
      }
    }

    if (!isEpmFound) {
      writer.Write(L"NewOS: No partition found for NewOS. (HCR-1000)\r\n");
    }

    handoverHdrPtr->f_Magic = kHandoverMagic;
    handoverHdrPtr->f_Version = kHandoverVersion;

    writer.Write(L"Running NewOS...\r\n");

    EFI::ExitBootServices(MapKey, ImageHandle);

    /// TODO: Read catalog and read NewKernel.exe

  } else {
    writer.Write(L"NewOS: Error-Code: HLDR-0003\r\n");
  }

  EFI::Stop();

  return kEfiFail;
}
