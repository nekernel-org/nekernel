/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include "EFIKit/EFI.hxx"
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

  BFileReader reader(L"EFI\\BOOT\\HCoreKrnl.exe");

  SizeT sz = 0UL;

  auto blob = reader.ReadAll(sz);

  if (!blob || sz < 1)
    KeRuntimeStop(L"HCoreLdr_NoSuchKernel",
                  L"Couldn't find HCoreKrnl.exe! Aborting...");

  writer.WriteString(L"HCoreLdr: Running HCoreKrnl.exe...\r\n");

  EFI::ExitBootServices(SystemTable, mapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
