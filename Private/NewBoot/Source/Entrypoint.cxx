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
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

  writer.WriteString(L"HCoreLdr: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  BFileReader img(L"HCOREKRNL.EXE", ImageHandle);

  UInt8 buf[1024] = {0};
  UInt32 bufSz = 1024;

  img.File()->Read(img.File(), &bufSz, buf);

  if (buf[0] != kMagMz0 || buf[1] != kMagMz1) {
    EFI::RaiseHardError(L"HCoreLdr_NoBlob", L"No Such blob.");
    return kEfiFail;
  }

  writer.WriteString(L"HCoreLdr: Loading HCOREKRNL.EXE...\r\n");

  UInt64 MapKey = 0;

  EFI::ExitBootServices(MapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
