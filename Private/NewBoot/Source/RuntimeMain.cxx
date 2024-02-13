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

#define kBufferReadSz 2048

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

#ifndef __DEBUG__

  writer.WriteString(
      L"MahroussLogic (R) HCore Version 1.00 (Release Channel)\r\n");

#else

  writer.WriteString(
      L"MahroussLogic (R) HCore Version 1.00 (Insiders Channel)\r\n");

#endif

  const char strDate[] = __DATE__;

  writer.WriteString(L"Build Date: ");

  for (auto& ch : strDate) {
    writer.WriteCharacter(ch);
  }

  writer.WriteString(L"\r\nHCoreLdr: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  writer.WriteString(L"HCoreLdr: Reading: ")
      .WriteString(L"HCOREKRNL.EXE")
      .WriteString(L"\r\n");

  BFileReader img(L"HCOREKRNL.EXE", ImageHandle);

  img.Size() = kBufferReadSz;
  img.Read();

  if (img.Error() == BFileReader::kOperationOkay) {
    BlobType blob = (BlobType)img.Blob();

    ExecHeaderPtr ptrHdr = reinterpret_cast<ExecHeaderPtr>(
        HCore::rt_find_exec_header(reinterpret_cast<DosHeaderPtr>(blob)));

    if (ptrHdr && ptrHdr->mMachine == EFI::Platform() &&
        ptrHdr->mMagic == kPeMagic) {
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
