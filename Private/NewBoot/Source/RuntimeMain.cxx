/*
 * ========================================================
 *
 *      NewBoot
 *      Copyright Mahrouss Logic, all rights reserved.
 *
 * ========================================================
 */

#include "EFIKit/EFI.hxx"
#define __BOOTLOADER__ 1

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

#ifdef __x86_64__

#include <HALKit/AMD64/HalPageAlloc.hpp>

#else

#error Unknown CPU.

#endif

#define kBufferReadSz 4096

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

#ifndef __DEBUG__

  writer.WriteString(L"HCoreLdr: Version 1.00 (Release Channel)\r\n");

#else

  writer.WriteString(L"HCoreLdr: Version 1.00 (Insiders Channel)\r\n");

#endif

  const char strDate[] = __DATE__;

  writer.WriteString(L"HCoreLdr: Build date: ");

  for (auto& ch : strDate) {
    writer.WriteCharacter(ch);
  }

  writer.WriteString(L"\r\nHCoreLdr: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  BFileReader img(L"HCOREKRNL.EXE", ImageHandle);

  img.Size(kBufferReadSz);
  img.Read();

  if (img.Error() == BFileReader::kOperationOkay) {
    BlobType blob = (BlobType)img.Blob();

    ExecHeaderPtr ptrHdr = reinterpret_cast<ExecHeaderPtr>(
        HCore::rt_find_exec_header(reinterpret_cast<DosHeaderPtr>(blob)));

    if (ptrHdr && ptrHdr->mMachine == EFI::Platform() &&
        ptrHdr->mMagic == kPeMagic) {
      if (ptrHdr->mNumberOfSections > 1) {
        ExecOptionalHeaderPtr optHdr = reinterpret_cast<ExecOptionalHeaderPtr>(
            ptrHdr + sizeof(ExecHeader));

        UInt64 baseSec = ptrHdr->mNumberOfSections;
        img.File()->SetPosition(img.File(), &baseSec);

        baseSec = ptrHdr->mNumberOfSections;
        ExecSectionHeaderPtr headers =
            (ExecSectionHeaderPtr)(&ptrHdr->mCharacteristics + 1);

        for (int i = 0u; i < ptrHdr->mNumberOfSections; ++i) {
          auto& hdr = headers[i];

          if (hdr.mName[0] != '.') continue;

          UInt64 addr = hdr.mVirtualAddress;

          BS->AllocatePages(AllocateAnyPages, EfiLoaderCode, 1, &addr);

          UInt64 pos = (optHdr->mImageBase + optHdr->mBaseOfData) +
                       hdr.mPointerToRawData;

          img.File()->SetPosition(img.File(), &pos);
          img.Size(hdr.mSizeOfRawData);
          img.File()->Read(img.File(), &img.Size(), (VoidPtr)addr);
        }

        UInt32 MapKey = 0;
        UInt32* Size = 0;
        EfiMemoryDescriptor* Descriptor = nullptr;
        UInt32 SzDesc = 0;
        UInt32 RevDesc = 0;

        if (BS->AllocatePool(EfiLoaderData, sizeof(UInt64), (VoidPtr*)&Size) !=
            kEfiOk) {
          EFI::RaiseHardError(L"HCoreLdr-BadAlloc",
                              L"Bad Alloc! (AllocatePool)");
        }

        *Size = sizeof(EfiMemoryDescriptor);

        if (BS->AllocatePool(EfiLoaderData, sizeof(EfiMemoryDescriptor),
                             (VoidPtr*)&Descriptor) != kEfiOk) {
          EFI::RaiseHardError(L"HCoreLdr-BadAlloc",
                              L"Bad Alloc! (AllocatePool)");
        }

        if (BS->GetMemoryMap(Size, Descriptor, &MapKey, &SzDesc, &RevDesc) !=
            kEfiOk) {
          EFI::RaiseHardError(L"HCoreLdr-BadAlloc",
                              L"Bad Alloc! (GetMemoryMap)");
        }

        HEL::HandoverInformationHeader* handoverHdrPtr = nullptr;

        BS->AllocatePool(EfiLoaderData, sizeof(HEL::HandoverInformationHeader),
                         (VoidPtr*)&handoverHdrPtr);

        handoverHdrPtr->f_GOP = (voidPtr)kGop->Mode->FrameBufferBase;
        handoverHdrPtr->f_GOPSize = kGop->Mode->FrameBufferSize;

        handoverHdrPtr->f_PhysicalStart =
            reinterpret_cast<voidPtr>(Descriptor->PhysicalStart);
        handoverHdrPtr->f_PhysicalSize = Descriptor->NumberOfPages * kPTESize;

        handoverHdrPtr->f_VirtualStart =
            reinterpret_cast<voidPtr>(Descriptor->VirtualStart);

        handoverHdrPtr->f_VirtualSize =
            Descriptor->NumberOfPages; /* # of pages */

        handoverHdrPtr->f_FirmwareVendorLen =
            BStrLen(SystemTable->FirmwareVendor);

        BCopyMem(handoverHdrPtr->f_FirmwareVendorName,
                 SystemTable->FirmwareVendor,
                 handoverHdrPtr->f_FirmwareVendorLen);

        handoverHdrPtr->f_HeapCommitSize = optHdr->mSizeOfHeapCommit;
        handoverHdrPtr->f_StackCommitSize = optHdr->mSizeOfStackCommit;

        EFI::ExitBootServices(MapKey, ImageHandle);

        HCore::HEL::HandoverProc proc =
            reinterpret_cast<HCore::HEL::HandoverProc>(
                optHdr->mAddressOfEntryPoint);

        proc(handoverHdrPtr);

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
