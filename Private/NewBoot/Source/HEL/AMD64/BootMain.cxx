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

#ifndef kBootKrnlSections
#error[NewBoot/NewBoot] Please provide the amount of sections the kernel has.
#endif  // !kBootKrnlSections

#define kBootReadSize \
  (sizeof(DosHeader) + sizeof(ExecHeader) + sizeof(ExecOptionalHeader))

EXTERN_C void Main(HEL::HandoverInformationHeader* HIH);

typedef void (*bt_main_type)(HEL::HandoverInformationHeader* HIH);

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);  // Init the efi library.
  InitGOP();             // Quick Toolkit for UI

  BTextWriter writer;

  /// Splash screen stuff

  writer.Write(L"MahroussLogic (R) NewBoot: ")
      .Write(BVersionString::Shared())
      .Write(L"\r\n");

  const char strDate[] = __DATE__;

  writer.Write(L"NewBoot: Build: ");

  for (auto& ch : strDate) writer.WriteCharacter(ch);

  writer.Write(L"\r\nNewBoot: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r\n");

  /// Read Kernel blob.

  BFileReader kernelImg(L".HCORE", ImageHandle);

  kernelImg.Size(kBootReadSize + sizeof(ExecSectionHeader) * kBootKrnlSections);
  kernelImg.ReadAll();

  if (kernelImg.Error() == BFileReader::kOperationOkay) {
    // first check for kernel.cfg inside ESP/EPM.
    BFileReader systemManifest(L".MANIFEST", ImageHandle);

    systemManifest.Size(1);
    systemManifest.ReadAll();

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

#ifndef __DEBUG__
    ST->ConOut->ClearScreen(ST->ConOut);
#endif

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

    handoverHdrPtr->f_HardwareTables.f_VendorTables =
        ST->ConfigurationTable->VendorTable;

    EFI::ExitBootServices(MapKey, ImageHandle);

    bool isIniNotFound = (systemManifest.Blob() == nullptr);

    if (isIniNotFound) {
      handoverHdrPtr->f_Magic = kHandoverMagic;
      handoverHdrPtr->f_Version = 0x1011;
      handoverHdrPtr->f_Bootloader = 0x11;  // Installer

      Main(handoverHdrPtr);
    } else {
      handoverHdrPtr->f_Magic = kHandoverMagic;
      handoverHdrPtr->f_Version = 0x1011;
      handoverHdrPtr->f_Bootloader = 0xDD;  // System present

      Main(handoverHdrPtr);
    }

    EFI::Stop();

    return kEfiOk;
  } else {
    writer.Write(L"NewBoot: Error-Code: HLDR-0003\r\n");
  }

  EFI::Stop();

  return kEfiFail;
}
