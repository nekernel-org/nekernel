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
#include <KernelKit/PE.hpp>
#include <NewKit/Ref.hpp>

STATIC Void InitGfx() noexcept {
  EfiGUID gopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  EfiGraphicsOutputProtocol* gop = nullptr;

  BS->LocateProtocol(&gopGuid, nullptr, (void**)&gop);

  for (int w = 0; w < gop->Mode->Info->VerticalResolution; ++w) {
    for (int h = 0; h < gop->Mode->Info->HorizontalResolution; ++h) {
      *((UInt32*)(gop->Mode->FrameBufferBase +
                  4 * gop->Mode->Info->PixelsPerScanLine * w + 4 * h)) =
          RGB(10, 10, 10);
    }
  }
}

EFI_EXTERN_C Int EfiMain(EfiHandlePtr ImageHandle,
                         EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);
  InitGfx();

  BTextWriter writer;

  writer.WriteString(L"HCoreLdr: ")
      .WriteString(L"Copyright Mahrouss-Logic Corporation.")
      .WriteString(L"\r\n");

  writer.WriteString(L"HCoreLdr: Firmware: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  UInt64 mapKey = 0;

  BFileReader img(L"\\EFI\\BOOT\\HCOREKRNL.EXE");

  SizeT imageSz = 0;

  PEImagePtr blob = (PEImagePtr)img.Fetch(ImageHandle, imageSz);

  if (!blob)
    EFI::RaiseHardError(L"HCoreLdr_NoSuchKernel",
                        L"Couldn't find HCoreKrnl.exe!");

  writer.WriteString(L"HCoreLdr: Running HCoreKrnl.exe...\r\n");

  EFI::ExitBootServices(mapKey, ImageHandle);
  EFI::Stop();

  return kEfiOk;
}
