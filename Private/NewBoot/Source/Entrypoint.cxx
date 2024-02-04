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

EFI_EXTERN_C Int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

  writer.WriteString(L"HCoreLdr: ")
      .WriteString(L"Copyright Mahrouss-Logic Corporation.")
      .WriteString(L"\r\n");

  writer.WriteString(L"HCoreLdr: Firmware: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  BFileReader img(L"\\EFI\\BOOT\\HCOREKRNL.EXE");
  img.Fetch(ImageHandle);

  VoidPtr blob = img.Blob();

  UInt64 MapKey = 0;

  EFI::ExitBootServices(MapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
