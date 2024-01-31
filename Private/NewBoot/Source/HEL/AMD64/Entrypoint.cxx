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
#include <EFIKit/EFILib.hxx>

// don't remove EfiGUID, it will call initializer_list!

EFI_EXTERN_C int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable* SystemTable) {
  KeInitEFI(SystemTable);

  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
  SystemTable->ConOut->SetAttribute(SystemTable->ConOut, kEFIYellow);

  BTextWriter writer;
  writer.WriteString(L"HCoreLdr: Booting from \\Volume0\\...")
      .WriteString(L"\r\n");

  UInt64 mapKey = 0;

  // TODO: Jump Code

  KeRuntimeStop(L"HCoreLdr", L"Couldn't find HCoreKrnl.exe! Aborting...");

  EFI::ExitBootServices(SystemTable, mapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
