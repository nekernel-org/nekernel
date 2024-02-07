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
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

namespace Detail {
constexpr Int32 kBufferReadSz = 2048;

auto FindPEHeader(DosHeaderPtr ptrDos) -> ExecHeaderPtr {
  if (!ptrDos) return nullptr;

  return (ExecHeaderPtr)(&ptrDos->eLfanew + 1);
}
}  // namespace Detail

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

#ifndef __DEBUG__

  writer.WriteString(
      L"MahroussLogic (R) HCore Version 1.0.0 (Release Channel)\r\n");

#else

  writer.WriteString(
      L"MahroussLogic (R) HCore Version 1.0.0 (Insiders Channel)\r\n");

#endif

  writer.WriteString(L"HCoreLdr: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  BFileReader img(L"HCOREKRNL.EXE", ImageHandle);

  img.Size() = Detail::kBufferReadSz;
  img.ReadAll();

  if (img.Error() == BFileReader::kOperationOkay) {
    BlobType blob = (BlobType)img.Blob();

    DosHeaderPtr ptrDos = reinterpret_cast<DosHeaderPtr>(blob);
    ExecHeaderPtr ptrHdr = Detail::FindPEHeader(ptrDos);

    if (ptrDos->eMagic[0] == kMagMz0 && ptrDos->eMagic[1] == kMagMz1 &&
        ptrHdr->mMachine == EFI::Platform() && ptrHdr->mMagic == kPeMagic) {
      if (ptrHdr->mNumberOfSections > 1) {
        UInt64 MapKey = 0;

        EFI::ExitBootServices(MapKey, ImageHandle);

        // Launch PE app.

        EFI::Stop();

        return kEfiOk;
      } else {
        writer.WriteString(L"HCoreLdr: Error-Code: HLDR-0001\r\n");
      }
    }
  } else {
    writer.WriteString(L"HCoreLdr: Error-Code: HLDR-0002\r\n");
  }

  EFI::Stop();

  return kEfiFail;
}
