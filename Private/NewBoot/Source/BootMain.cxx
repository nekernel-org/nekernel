/*
 * ========================================================
 *
 *      NewBoot
 *      Copyright Mahrouss Logic, all rights reserved.
 *
 * ========================================================
 */

#define __BOOTLOADER__ 1

#include <BootKit/BootKit.hxx>
#include <HALKit/AMD64/ACPI/ACPI.hpp>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#else
#error This CPU is unknown.
#endif  // ifdef __x86_64__

#define kBootReadSize \
  (sizeof(DosHeader) + sizeof(ExecHeader) + sizeof(ExecOptionalHeader))

EXTERN_C void Main(HEL::HandoverInformationHeader* HIH);

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

  writer.Write(L"HCoreLdr: ");

  writer.Write(BVersionString::Shared()).Write(L"\r\n");

  const char strDate[] = __DATE__;

  writer.Write(L"HCoreLdr: Build: ");

  for (auto& ch : strDate) writer.WriteCharacter(ch);

  writer.Write(L"\r\nHCoreLdr: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r\n");

  BFileReader img(L"HCOREKRNL.DLL", ImageHandle);

  img.Size(kBootReadSize);
  img.ReadAll();

  if (img.Error() == BFileReader::kOperationOkay) {
    BlobType blob = (BlobType)img.Blob();

    ExecHeaderPtr ptrHdr = reinterpret_cast<ExecHeaderPtr>(
        HCore::rt_find_exec_header(reinterpret_cast<DosHeaderPtr>(blob)));

    if (ptrHdr && ptrHdr->mMachine == EFI::Platform() &&
        ptrHdr->mMagic == kPeMagic) {
      /// sections must be at least 3.
      if (ptrHdr->mNumberOfSections >= 3) {
        ExecOptionalHeaderPtr optHdr = reinterpret_cast<ExecOptionalHeaderPtr>(
            ptrHdr + sizeof(ExecHeader));

        UInt32 MapKey = 0;
        UInt32* Size;
        EfiMemoryDescriptor* Descriptor;
        UInt32 SzDesc = 0;
        UInt32 RevDesc = 0;

        if (BS->AllocatePool(EfiLoaderData, sizeof(UInt32), (VoidPtr*)&Size) !=
            kEfiOk) {
          EFI::RaiseHardError(
              L"HCoreLdr-BadAlloc",
              L"The bootloader ran out of memory! Please check your specs.");
        }

        *Size = sizeof(EfiMemoryDescriptor);

        if (BS->AllocatePool(EfiLoaderData, sizeof(EfiMemoryDescriptor),
                             (VoidPtr*)&Descriptor) != kEfiOk) {
          EFI::RaiseHardError(
              L"HCoreLdr-BadAlloc",
              L"The bootloader ran out of memory! Please check your specs.");
        }

        if (BS->GetMemoryMap(Size, Descriptor, &MapKey, &SzDesc, &RevDesc) !=
            kEfiOk) {
          EFI::RaiseHardError(
              L"HCoreLdr-GetMemoryMap",
              L"GetMemoryMap returned a value which isn't kEfiOk!");
        }

        HEL::HandoverInformationHeader* handoverHdrPtr = nullptr;

        BS->AllocatePool(EfiLoaderData, sizeof(HEL::HandoverInformationHeader),
                         (VoidPtr*)&handoverHdrPtr);

        handoverHdrPtr->f_GOP.f_The = kGop->Mode->FrameBufferBase;
        handoverHdrPtr->f_GOP.f_Width = kGop->Mode->Info->VerticalResolution;
        handoverHdrPtr->f_GOP.f_Height = kGop->Mode->Info->HorizontalResolution;
        handoverHdrPtr->f_GOP.f_PixelPerLine =
            kGop->Mode->Info->PixelsPerScanLine;
        handoverHdrPtr->f_GOP.f_PixelFormat = kGop->Mode->Info->PixelFormat;
        handoverHdrPtr->f_GOP.f_Size = kGop->Mode->FrameBufferSize;

        handoverHdrPtr->f_PhysicalStart =
            reinterpret_cast<voidPtr>(Descriptor->PhysicalStart);
        handoverHdrPtr->f_PhysicalSize = Descriptor->NumberOfPages;

        handoverHdrPtr->f_VirtualStart =
            reinterpret_cast<voidPtr>(Descriptor->VirtualStart);

        handoverHdrPtr->f_VirtualSize =
            Descriptor->NumberOfPages; /* # of pages */

        handoverHdrPtr->f_FirmwareVendorLen =
            BStrLen(SystemTable->FirmwareVendor);

        BCopyMem(handoverHdrPtr->f_FirmwareVendorName,
                 SystemTable->FirmwareVendor,
                 handoverHdrPtr->f_FirmwareVendorLen);

        BFileReader systemIni(L"SYSTEM.INI", ImageHandle);

        systemIni.Size(1);
        systemIni.ReadAll();

        bool isIniNotFound = (systemIni.Blob() == nullptr);

        if (isIniNotFound) {
          handoverHdrPtr->f_Magic = 0x55DDFF;
          handoverHdrPtr->f_Version = 0x1011;
          handoverHdrPtr->f_Bootloader = 0x11;  // Installer

          writer.Write(L"HCoreLdr: Loading HCore...\r\n");

          EFI::ExitBootServices(MapKey, ImageHandle);

          Main(handoverHdrPtr);

        } else {
          handoverHdrPtr->f_Magic = 0xFF55DD;
          handoverHdrPtr->f_Version = 0x1011;
          handoverHdrPtr->f_Bootloader = 0xDD;  // System present

          // TODO: read .NewBoot section.
        }

        EFI::Stop();

        return kEfiOk;
      } else {
        writer.Write(L"HCoreLdr: Error-Code: HLDR-0001\r\n");
      }
    } else {
      writer.Write(L"HCoreLdr: Error-Code: HLDR-0002\r\n");
    }
  } else {
    writer.Write(L"HCoreLdr: Error-Code: HLDR-0003\r\n");
  }

  EFI::Stop();

  return kEfiFail;
}
