/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include <BootKit/Boot.hxx>
#include <BootKit/Processor.hxx>
#include <BootKit/Protocol.hxx>

EFI_EXTERN_C int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable *SystemTable) {
  SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                    L"HCoreLdr: Starting \\EPM\\HCore...\r\n");

  if (SystemTable->BootServices->ExitBootServices(
          ImageHandle, kBaseHandoverStruct) != kEfiOk) {
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"HCoreLdr: Could not Exit UEFI!\r\nHanging...\r\n");

    return kEfiFail;
  }

  return kEfiOk;
}
