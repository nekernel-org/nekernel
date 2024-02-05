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

  BFileReader img(L"HCOREKRNL.EXE");
  img.ReadAll(ImageHandle);

  VoidPtr blob = img.Blob();

  if (!blob) {
    EFI::RaiseHardError(L"HCoreLdr_NoBlob", L"No Such blob.");
    return kEfiFail;
  }

  UInt64 MapKey = 0;

  EFI::ExitBootServices(MapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
