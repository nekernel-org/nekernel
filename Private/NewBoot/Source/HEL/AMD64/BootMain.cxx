/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/Driver.rsrc>
#include <Builtins/Toolbox/Lerp.hxx>
#include <Builtins/Toolbox/Toolbox.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PEF.hpp>
#include <NewKit/Ref.hpp>

/** Graphics related. */

STATIC EfiGraphicsOutputProtocol* kGop = nullptr;
STATIC UInt16 kStride = 0U;
STATIC EfiGUID kGopGuid;

/**
    @brief Finds and stores the GOP.
*/

STATIC Void CheckAndFindFramebuffer() noexcept {
  kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  kGop = nullptr;

  extern EfiBootServices* BS;

  BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*)&kGop);

  kStride = 4;
}

/// @brief check the BootDevice if suitable.
STATIC Bool CheckBootDevice(BootDeviceATA& ataDev) {
  if (ataDev.Leak().mErr) return false;
  return true;
}

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return
EFI_EXTERN_C EFI_API Int Main(EfiHandlePtr ImageHandle,
                              EfiSystemTable* SystemTable) {
  InitEFI(SystemTable);       ///! Init the EFI library.
  CheckAndFindFramebuffer();  ///! Init the GOP.

  BTextWriter writer;

  /// Splash screen stuff

  writer.Write(L"Mahrouss-Logic (R) New Boot: ")
      .Write(BVersionString::Shared());

  writer.Write(L"\r\nNewBoot: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r\n");

  BootDeviceATA ataDev;
  Boolean isGptFound = No;

  UInt32 MapKey = 0;
  UInt32* SizePtr = nullptr;
  EfiMemoryDescriptor* Descriptor = nullptr;
  UInt32 SzDesc = 0;
  UInt32 RevDesc = 0;

  if (BS->AllocatePool(EfiLoaderData, sizeof(UInt32), (VoidPtr*)&SizePtr) !=
      kEfiOk) {
    EFI::RaiseHardError(L"Bad-Alloc", L"New Boot ran out of memory!");
  }

  /****
   *
   *  Load kernel into memory.
   *
   */

  *SizePtr = sizeof(EfiMemoryDescriptor);

  if (BS->AllocatePool(EfiLoaderData, sizeof(EfiMemoryDescriptor),
                       (VoidPtr*)&Descriptor) != kEfiOk) {
    EFI::RaiseHardError(L"Bad-Alloc", L"New Boot ran out of memory!");
  }

  HEL::HandoverInformationHeader* handoverHdrPtr = nullptr;

  for (SizeT indexVT = 0; indexVT < SystemTable->NumberOfTableEntries;
       ++indexVT) {
    volatile Char* vendorTable = reinterpret_cast<volatile Char*>(
        SystemTable->ConfigurationTable[indexVT].VendorTable);

    if (vendorTable[0] == 'R' && vendorTable[1] == 'S' &&
        vendorTable[2] == 'D' && vendorTable[3] == ' ' &&
        vendorTable[4] == 'P' && vendorTable[5] == 'T' &&
        vendorTable[6] == 'R' && vendorTable[7] == ' ') {
      handoverHdrPtr->f_HardwareTables.f_RsdPtr = (VoidPtr)vendorTable;

      break;
    }
  }

  handoverHdrPtr->f_Magic = kHandoverMagic;
  handoverHdrPtr->f_Version = kHandoverVersion;

  BCopyMem(handoverHdrPtr->f_FirmwareVendorName, SystemTable->FirmwareVendor,
           handoverHdrPtr->f_FirmwareVendorLen);

  handoverHdrPtr->f_GOP.f_The = kGop->Mode->FrameBufferBase;
  handoverHdrPtr->f_GOP.f_Width = kGop->Mode->Info->VerticalResolution;
  handoverHdrPtr->f_GOP.f_Height = kGop->Mode->Info->HorizontalResolution;
  handoverHdrPtr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
  handoverHdrPtr->f_GOP.f_PixelFormat = kGop->Mode->Info->PixelFormat;
  handoverHdrPtr->f_GOP.f_Size = kGop->Mode->FrameBufferSize;

  ///! Finally draw bootloader screen.

  auto kHandoverHeader = handoverHdrPtr;

  ToolboxInitRsrc();

  ToolboxDrawZone(RGB(FF, FF, FF), handoverHdrPtr->f_GOP.f_Height,
                  handoverHdrPtr->f_GOP.f_Width, 0, 0);

  ToolboxClearRsrc();

  ToolboxDrawRsrc(Driver, DRIVER_HEIGHT, DRIVER_WIDTH,
                  (handoverHdrPtr->f_GOP.f_Width - DRIVER_HEIGHT) / 2,
                  (handoverHdrPtr->f_GOP.f_Height - DRIVER_HEIGHT) / 2);

  ToolboxClearRsrc();

  BS->AllocatePool(EfiLoaderData, sizeof(HEL::HandoverInformationHeader),
                   (VoidPtr*)&handoverHdrPtr);

  handoverHdrPtr->f_PhysicalStart = 0;
  handoverHdrPtr->f_PhysicalSize = 0;

  EfiPhysicalAddress* whereAddress =
      reinterpret_cast<EfiPhysicalAddress*>(kBootVirtualAddress);

  BS->AllocatePages(EfiAllocateType::AllocateAnyPages,
                    EfiMemoryType::EfiConventionalMemory, 1, whereAddress);

  handoverHdrPtr->f_VirtualStart = reinterpret_cast<voidPtr>(whereAddress);

  handoverHdrPtr->f_VirtualSize = Descriptor->NumberOfPages; /* # of pages */

  handoverHdrPtr->f_FirmwareVendorLen = BStrLen(SystemTable->FirmwareVendor);

  BFileReader reader(L"SplashScreen.fmt", ImageHandle);
  reader.ReadAll(512, 16);

  if (reader.Blob()) {
    Char* buf = (Char*)reader.Blob();

    for (SizeT i = 0; i < reader.Size(); ++i) {
      if (buf[i] != '\n' && buf[i] != '\r') {
        if (buf[i] == '*') {
          writer.WriteCharacter('\t');
        } else {
          writer.WriteCharacter(buf[i]);
        }
      } else
        writer.Write(L"\r\n");
    }
  }

  BFileReader kernelFile(L"NewLoader.exe", ImageHandle);
  kernelFile.ReadAll(MIB(1), 4096);

  if (kernelFile.Blob()) {
    PEFContainer* headerKind =
        reinterpret_cast<PEFContainer*>(kernelFile.Blob());

    if (headerKind->Magic[0] == kPefMagic[0] &&
        headerKind->Magic[1] == kPefMagic[1] &&
        headerKind->Magic[2] == kPefMagic[2] &&
        headerKind->Magic[3] == kPefMagic[3] &&
        headerKind->Magic[4] == kPefMagic[4]) {
      if (headerKind->Abi != kPefAbi || headerKind->Cpu != kPefArchAMD64) {
        EFI::RaiseHardError(L"Bad-Architecture",
                            L"New Boot can't run this architecture.");
      }

      BootMainKind main = (BootMainKind) nullptr;

      if (!main) {
        EFI::RaiseHardError(L"Bad-Exec",
                            L"New Boot can't recognize this executable.");
      }

      EFI::ExitBootServices(MapKey, ImageHandle);

      main(handoverHdrPtr);

      EFI::Stop();

      CANT_REACH();
    }
  }

  return kEfiFail;
}
