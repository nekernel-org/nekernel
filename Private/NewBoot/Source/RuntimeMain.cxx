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
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

EFI_EXTERN_C EFI_API Int EfiMain(EfiHandlePtr ImageHandle,
                                 EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitQT();

  BTextWriter writer;

#ifndef __DEBUG__

  writer.WriteString(
      L"MahroussLogic (R) HCore Version 1.0.0 (Release Channel)\r\n");

#else

  writer.WriteString(
      L"MahroussLogic (R) HCore Version 1.0.0 (Insiders Channel)\r\n");

#endif

  writer.WriteString(L"HCoreLdr: Firmware Vendor: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  BFileReader img(L"HCOREKRNL.EXE", ImageHandle);
  img.ReadAll();

  if (img.Error() == BFileReader::kOperationOkay) {
    VoidPtr blob = img.Blob();

    UInt64 MapKey = 0;

    EFI::ExitBootServices(MapKey, ImageHandle);
    EFI::Stop();

    return kEfiOk;
  }

  writer.WriteString(
      L"HCoreLdr: Missing HCOREKRNL.EXE! Your system is damaged.\r\n");

  EFI::Stop();

  return kEfiFail;
}
