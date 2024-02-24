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
#include <EFIKit/Api.hxx>
#include <HALKit/AMD64/ACPI/ACPI.hpp>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#else
#error This CPU is unknown.
#endif  // ifdef __x86_64__

#define kHeadersSz \
  (sizeof(DosHeader) + sizeof(ExecHeader) + sizeof(ExecOptionalHeader))

#ifdef __BUNDLE_KERNEL__
EXTERN_C EFI_API void RuntimeMain(HEL::HandoverInformationHeader* HIH);
#endif

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

#ifdef __BUNDLE_KERNEL__
  writer.WriteString(L"HCoreLite: ");
#else
  writer.WriteString(L"HCoreLdr:");
#endif

#ifndef __DEBUG__

  writer.WriteString(L"Version 1.00 (Release Channel)\r\n");

#else

  writer.WriteString(L"Version 1.00 (Insiders Channel)\r\n");

#endif

  const char strDate[] = __DATE__;

#ifdef __BUNDLE_KERNEL__
  writer.WriteString(L"HCoreLite: Build: ");
#else
  writer.WriteString(L"HCoreLdr: Build: ");
#endif

  for (auto& ch : strDate) writer.WriteCharacter(ch);

#ifdef __BUNDLE_KERNEL__
  writer.WriteString(L"\r\nHCoreLite: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");
#else
  writer.WriteString(L"\r\nHCoreLdr: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");
#endif

  BFileReader img(L"HCOREKRNL.DLL", ImageHandle);

  img.Size(kHeadersSz);
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

        if (optHdr->mSubsystem != 0xAFAF) {
          writer.WriteString(L"HCoreLdr: This is not an HCore app: Subsystem: ")
              .WriteString(optHdr->mSubsystem)
              .WriteString(L"\r\n");
          EFI::Stop();
        }

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

#ifdef __BUNDLE_KERNEL__
        handoverHdrPtr->f_Magic = 0x55DDFF;
        handoverHdrPtr->f_Version = 0x1011;
        handoverHdrPtr->f_Bootloader = 0x11;  // HCoreLite

        writer.WriteString(L"HCoreLite: Exit Boot...").WriteString(L"\r\n");
#else
        handoverHdrPtr->f_Magic = 0xFF55DD;
        handoverHdrPtr->f_Version = 0x1011;
        handoverHdrPtr->f_Bootloader = 0xDD;  // HCoreLdr

        writer.WriteString(L"HCoreLdr: Exit Boot...").WriteString(L"\r\n");
#endif

        EFI::ExitBootServices(MapKey, ImageHandle);

#ifdef __BUNDLE_KERNEL__
        RuntimeMain(handoverHdrPtr);
#else
        // Load HCoreKrnl.dll (TODO)

#endif  // ifdef __BUNDLE_KERNEL__

        EFI::Stop();

        return kEfiOk;
      } else {
        writer.WriteString(L"HCoreLdr: Error-Code: HLDR-0001\r\n");
      }
    } else {
      writer.WriteString(L"HCoreLdr: Error-Code: HLDR-0002\r\n");
    }
  } else {
    writer.WriteString(L"HCoreLdr: Error-Code: HLDR-0003\r\n");
  }

  EFI::Stop();

  return kEfiFail;
}
