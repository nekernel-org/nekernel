/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include <BootKit/Boot.hxx>

namespace Detail {
/**
@brief Stop Execution of Bootloader.
@param SystemTable EFI System Table.
*/
Void Stop(EfiSystemTable* SystemTable) noexcept {
  SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                    L"HCoreLdr: Hanging...\r\n");

  while (true) {
    rt_cli();
    rt_halt();
  }
}
}  // namespace Detail

// don't remove EfiGUID, it will call initializer_list!

EFI_EXTERN_C int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable* SystemTable) {
  SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                    L"HCoreLdr: Initializing...\r\n");

  EfiLoadImageProtocol* protocol = nullptr;
  EfiGUID guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

  Int32 status_code = SystemTable->BootServices->OpenProtocol(
      ImageHandle, &guid, (VoidPtr*)&protocol, ImageHandle, nullptr,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL);

  if (status_code != kEfiOk) {
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
