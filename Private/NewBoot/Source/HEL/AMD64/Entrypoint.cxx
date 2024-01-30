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

  SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                    L"HCoreLdr: Initializing...\r\n");

  EfiLoadImageProtocol* protocol = nullptr;
  EfiGUID guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

  Int32 statusCode = SystemTable->BootServices->OpenProtocol(
      ImageHandle, &guid, (VoidPtr*)&protocol, ImageHandle, nullptr,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL);

  if (statusCode != kEfiOk) {
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"HCoreLdr: Could not locate EfiLoadImageProtocol! Aborting...\r\n");

    Detail::Stop(SystemTable);
    return kEfiFail;
  }

  SystemTable->BootServices->OpenProtocol(
      ImageHandle, &guid, (VoidPtr*)&protocol, ImageHandle, ImageHandle,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL);

  if (SystemTable->BootServices->ExitBootServices(ImageHandle, 0) != kEfiOk) {
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut, L"HCoreLdr: Could not exit Boot Services!\r\n");

    Detail::Stop(SystemTable);
  }

  // TODO: Jump Code

  Detail::Stop(SystemTable);
  return kEfiOk;
}
