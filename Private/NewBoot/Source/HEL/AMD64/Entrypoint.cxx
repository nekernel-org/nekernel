/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
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

  BTextWriter writer;
  writer.WriteString(L"HCoreLdr: Booting from disk...\r\n");

  // TODO: Jump Code

  EFI::Stop(SystemTable);
  return kEfiOk;
}
