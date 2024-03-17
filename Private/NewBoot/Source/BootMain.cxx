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
#error [NewBoot/HCoreLdr] Please provide the amount of sections the kernel has.
#endif  // !kBootKrnlSections

#define kBootReadSize \
  (sizeof(DosHeader) + sizeof(ExecHeader) + sizeof(ExecOptionalHeader))

EXTERN_C void Main(HEL::HandoverInformationHeader* HIH);

typedef void (*bt_main_type)(HEL::HandoverInformationHeader* HIH);

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable); // Init the efi library.
  InitGOP(); // Quick Toolkit for UI

  BTextWriter writer;

  /// Splash screen stuff

  writer.Write(L"MahroussLogic (R) HCoreLdr: ")
        .Write(BVersionString::Shared()).Write(L"\r\n");

  const char strDate[] = __DATE__;

  writer.Write(L"HCoreLdr: Build: ");

  for (auto& ch : strDate) writer.WriteCharacter(ch);

  writer.Write(L"\r\nHCoreLdr: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r\n");

  /// Read Kernel blob.

  BFileReader kernelImg(L"HCOREKRNL.EXE", ImageHandle);

  kernelImg.Size(kBootReadSize);
  kernelImg.ReadAll();

  if (kernelImg.Error() == BFileReader::kOperationOkay) {
    BlobType blob = (BlobType)kernelImg.Blob();

    ExecHeaderPtr ptrHdr = reinterpret_cast<ExecHeaderPtr>(
        HCore::rt_find_exec_header(reinterpret_cast<DosHeaderPtr>(blob)));

    if (ptrHdr && ptrHdr->mMachine == EFI::Platform() &&
        ptrHdr->mMagic == kPeMagic) {
      /// sections must be at least 3.
      if (ptrHdr->mNumberOfSections == kBootKrnlSections) {
        ExecOptionalHeaderPtr optHdr = reinterpret_cast<ExecOptionalHeaderPtr>(
            (UIntPtr)&ptrHdr->mCharacteristics + 1);

        // first check for kernel.cfg inside ESP/EPM.
        BFileReader systemIni(L"KERNEL.CFG", ImageHandle);

        systemIni.Size(1);
        systemIni.ReadAll();

        bt_main_type kernelMain =
            (bt_main_type)(UIntPtr)optHdr->mAddressOfEntryPoint;

        SizeT heapSize = optHdr->mSizeOfHeapReserve;
        SizeT stackSize = optHdr->mSizeOfStackReserve;

        UInt64 posSeek = 0;

        // load sections here

        kernelImg.File()->SetPosition(kernelImg.File(), &posSeek);
        kernelImg.Size(kBootReadSize +
                       ptrHdr->mNumberOfSections * sizeof(ExecSectionHeader));

        kernelImg.ReadAll();

        writer.Write(L"HCoreLdr: Number of sections: ")
            .Write(ptrHdr->mNumberOfSections)
            .Write(L"\r\n");
        writer.Write(L"HCoreLdr: Size of sections: ")
            .Write(ptrHdr->mNumberOfSections * sizeof(ExecSectionHeader))
            .Write(L"\r\n");

        ExecSectionHeader* blobKrnl =
            (ExecSectionHeader*)(&optHdr + sizeof(ExecOptionalHeader) + sizeof(UInt32));

        for (size_t i = 0; i < ptrHdr->mNumberOfSections; i++) {
          // TODO: parse PE information here.
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

        /****
         * 
         *  Load kernel into memory.
         *
        */

        *Size = sizeof(EfiMemoryDescriptor);

        if (BS->AllocatePool(EfiLoaderData, sizeof(EfiMemoryDescriptor),
                             (VoidPtr*)&Descriptor) != kEfiOk) {
          EFI::RaiseHardError(
              L"HCoreLdr-BadAlloc",
              L"The bootloader ran out of memory! Please check your specs.");
        }

        /****
         * 
         *  Get machine memory map.
         * 
        */

        if (BS->GetMemoryMap(Size, Descriptor, &MapKey, &SzDesc, &RevDesc) !=
            kEfiOk) {
          EFI::RaiseHardError(
              L"HCoreLdr-GetMemoryMap",
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

        handoverHdrPtr->f_HardwareTables.f_VendorTables = ST->ConfigurationTable->VendorTable;

        EFI::ExitBootServices(MapKey, ImageHandle);

        bool isIniNotFound = (systemIni.Blob() == nullptr);

        if (isIniNotFound) {
          handoverHdrPtr->f_Magic = kHandoverMagic;
          handoverHdrPtr->f_Version = 0x1011;
          handoverHdrPtr->f_Bootloader = 0x11;  // Installer

          Main(handoverHdrPtr);
        } else {
          handoverHdrPtr->f_Magic = kHandoverMagic;
          handoverHdrPtr->f_Version = 0x1011;
          handoverHdrPtr->f_Bootloader = 0xDD;  // System present

          MUST_PASS(kernelMain);

          kernelMain(handoverHdrPtr);
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
