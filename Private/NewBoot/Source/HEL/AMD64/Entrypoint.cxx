/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#define __BOOTLOADER__ 1

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>

// don't remove EfiGUID, it will call initializer_list!

EFI_EXTERN_C int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable* SystemTable) {
  KeInitEFI(SystemTable);

  BTextWriter writer;

  writer.WriteString(L"HCoreLdr: Firmware: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  UInt64 mapKey = 0;

  BFileReader reader(L"\\Root\\System\\HCoreKrnl.exe\0");
  auto blob = reader.ReadAll();

  if (!blob)
    KeRuntimeStop(L"HCoreLdr_NoSuchKernel",
                  L"Couldn't find HCoreKrnl.exe! Aborting...");

  EFI::ExitBootServices(SystemTable, mapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
