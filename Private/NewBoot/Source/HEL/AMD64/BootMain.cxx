/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#else
#error This CPU is unknown.
#endif  // ifdef __x86_64__

#define kMaxBufSize 256

EXTERN_C void Main(HEL::HandoverInformationHeader* handoverInfo);

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

  writer.Write(L"MahroussLogic (R) NewBoot.exe: ")
      .Write(BVersionString::Shared());

  writer.Write(L"\r\nNewBoot.exe: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r\n");

  BootDeviceATA ataDrv;
  Boolean isIniNotFound = No;

  /// if ATA drive is initialized and EFI vendor supports an EPM scheme.
  /// @EDK tells our OS that it supports EPM scheme as well.
  if (ataDrv &&
      SystemTable->FirmwareVendor[0] == '@') {
    Char namePart[kEPMNameLength] = {"BootBlock"};

    /// tries to read an EPM block, or writes one if it fails.
    bool isIniNotFound =
        boot_write_epm_partition(namePart, kEPMNameLength, &ataDrv);
  } else if (SystemTable->FirmwareVendor[0] != '@') {
    writer.Write(L"This firmware can't understand NewOS, please use Mahrouss Logic products instead\r\nOur website: www.el-mahrouss-logic.com\r\n");
    return kEfiFail;
  }

  /// Read Kernel blob.

  BFileReader kernelImg(L".HCORE", ImageHandle);

  kernelImg.Size(kMaxBufSize);
  kernelImg.ReadAll();

  if (kernelImg.Error() == BFileReader::kOperationOkay) {
    UInt32 MapKey = 0;
    UInt32* Size;
    EfiMemoryDescriptor* Descriptor;
    UInt32 SzDesc = 0;
    UInt32 RevDesc = 0;

    if (BS->AllocatePool(EfiLoaderData, sizeof(UInt32), (VoidPtr*)&Size) !=
        kEfiOk) {
      EFI::RaiseHardError(
          L"NewBoot-BadAlloc",
          L"NewBoot ran out of memory! Please check your specs.");
    }

    /****
     *
     *  Load kernel into memory.
     *
     */

    *Size = sizeof(EfiMemoryDescriptor);

    if (BS->AllocatePool(EfiLoaderData, sizeof(EfiMemoryDescriptor),
                         (VoidPtr*)&Descriptor) != kEfiOk) {
      EFI::RaiseHardError(
          L"NewBoot-BadAlloc",
          L"NewBoot ran out of memory! Please check your specs.");
    }

    /****
     *
     *  Get machine memory map.
     *
     */

    if (BS->GetMemoryMap(Size, Descriptor, &MapKey, &SzDesc, &RevDesc) !=
        kEfiOk) {
      EFI::RaiseHardError(L"NewBoot-GetMemoryMap",
                          L"GetMemoryMap returned a value which isn't kEfiOk!");
    }

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
    writer.Write(L"NewBoot.exe: Fetching ACPI's 'RSD PTR'...").Write(L"\r\n");
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
        writer
            .Write(
                L"NewBoot.exe: Found ACPI's 'RSD PTR' table on this machine.")
            .Write(L"\r\n");
#endif

        break;
      }
    }

    if (!isIniNotFound) {
      writer.Write(L"NewBoot.exe: No partition found for NewOS. (HCR-1000)\r\n");
      writer.Write(L"NewBoot.exe: Running setup for NewOS...\r\n");
      
      EFI::ExitBootServices(MapKey, ImageHandle);

      Main(handoverHdrPtr);
    } else {
      handoverHdrPtr->f_Magic = kHandoverMagic;
      handoverHdrPtr->f_Version = kHandoverVersion;

      writer.Write(L"NewBoot.exe: Running NewOS...\r\n");

      EFI::ExitBootServices(MapKey, ImageHandle);

      /// TODO: Read catalog and read NewKernel.exe
    }

    EFI::Stop();

    return kEfiOk;
  } else {
    writer.Write(L"NewBoot.exe: Error-Code: HLDR-0003\r\n");
  }

  EFI::Stop();

  return kEfiFail;
}
