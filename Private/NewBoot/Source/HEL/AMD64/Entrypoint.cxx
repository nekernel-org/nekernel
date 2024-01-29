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

// don't remove EfiGUID, it will call initializer_list!

EFI_EXTERN_C int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable* SystemTable) {
  EfiCharType* string = L"HCoreLdr: Initializing...\r\n";

  SystemTable->ConOut->OutputString(SystemTable->ConOut, string);

  EfiLoadImageProtocol* protocol;
  EfiGUID guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

  SystemTable->BootServices->LocateProtocol(&guid, nullptr,
                                            (VoidPtr*)&protocol);

  if (!protocol) {
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"HCoreLdr: Could not locate EfiLoadImageProtocol! Aborting...\r\n");

    return kEfiFail;
  }

  SystemTable->BootServices->OpenProtocol(
      ImageHandle, &guid, (VoidPtr*)&protocol, ImageHandle, ImageHandle,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL);

  if (SystemTable->BootServices->ExitBootServices(ImageHandle, 0) != kEfiOk) {
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut, L"HCoreLdr: Could not exit Boot Services!\r\n");

    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                      L"HCoreLdr: Hanging...\r\n");

    while (true) {
      rt_cli();
      rt_halt();
    }
  }

  // TODO: Jump Code

  while (true) {
    rt_cli();
    rt_halt();
  }

  return kEfiOk;
}
