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

STATIC Void DrawBackground() {
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
  DrawBackground();

  BTextWriter writer;

  writer.WriteString(L"HCoreLdr: Firmware: ")
      .WriteString(SystemTable->FirmwareVendor)
      .WriteString(L"\r\n");

  UInt64 mapKey = 0;

  BFileReader img(L"HCoreKrnl.exe");

  SizeT sz = 0UL;
  PEImagePtr blob = (PEImagePtr)img.Fetch(sz);

  if (!blob || sz < 1)
    EFI::RaiseHardError(L"HCoreLdr_NoSuchKernel",
                        L"Couldn't find HCoreKrnl.exe!");

  ExecHeaderPtr headerPtr = (ExecHeaderPtr)blob;

  if (blob[0] == kMagMz0 && blob[1] == kMagMz1) {
    writer.WriteString(L"HCoreLdr: Running HCoreKrnl.exe...\r\n");
    /// Load Image here
  } else {
    EFI::RaiseHardError(L"HCoreLdr_NotPE", L"Not a PE file! Aborting...");
  }

  EFI::Stop();
  EFI::ExitBootServices(mapKey, ImageHandle);

  return kEfiOk;
}
