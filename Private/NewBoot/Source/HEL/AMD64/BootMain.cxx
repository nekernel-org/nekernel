/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/Rsrc/NewBoot.rsrc>
#include <Builtins/Toolbox/Toolbox.hxx>
#include <FirmwareKit/EFI.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PEF.hpp>
#include <NewKit/Macros.hpp>
#include <BootKit/BootKit.hxx>
#include <NewKit/Ref.hpp>
#include <FirmwareKit/Handover.hxx>
#include <cstring>

/// make the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "NeWS HD"
#endif // !kMachineModel

/** Graphics related. */

STATIC EfiGraphicsOutputProtocol* kGop = nullptr;
STATIC UInt16 kStride = 0U;
STATIC EfiGUID kGopGuid;

EXTERN_C Void hal_init_platform(HEL::HandoverInformationHeader* HIH);

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

  writer.Write(L"\rNew Boot: Firmware Vendor: ")
      .Write(SystemTable->FirmwareVendor)
      .Write(L"\r");

  UInt32* MapKey = new UInt32();
  UInt32* SizePtr = new UInt32();
  EfiMemoryDescriptor* Descriptor = new EfiMemoryDescriptor();
  UInt32* SzDesc = new UInt32();
  UInt32* RevDesc = new UInt32();

  *MapKey = 0;
  *SizePtr = sizeof(EfiMemoryDescriptor);

  HEL::HandoverInformationHeader* handoverHdrPtr = new HEL::HandoverInformationHeader();

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

  kHandoverHeader = handoverHdrPtr;

  ToolboxInitRsrc();

  ToolboxDrawZone(RGB(9d, 9d, 9d), handoverHdrPtr->f_GOP.f_Height,
                  handoverHdrPtr->f_GOP.f_Width, 0, 0);

  ToolboxClearRsrc();

  ToolboxDrawRsrc(NewBoot, NEWBOOT_HEIGHT, NEWBOOT_WIDTH,
                  (handoverHdrPtr->f_GOP.f_Width - NEWBOOT_WIDTH) / 2,
                  (handoverHdrPtr->f_GOP.f_Height - NEWBOOT_HEIGHT) / 2);

  ToolboxClearRsrc();

  BS->GetMemoryMap(SizePtr, Descriptor, MapKey, SzDesc, RevDesc);

  handoverHdrPtr->f_PhysicalStart = (VoidPtr)Descriptor->PhysicalStart;

  handoverHdrPtr->f_FirmwareSpecific[HEL::kHandoverSpecificAttrib] = Descriptor->Attribute;
  handoverHdrPtr->f_FirmwareSpecific[HEL::kHandoverSpecificKind] = Descriptor->Kind;

  handoverHdrPtr->f_VirtualStart = (VoidPtr)Descriptor->VirtualStart;
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
        writer.Write(L"\r");
    }
  }

  ///
  /// The following checks for an exisiting partition
  /// inside the disk, if it doesn't have one,
  /// format the disk.
  //

  BDiskFormatFactory<BootDeviceATA> diskFormatter;

  /// if not formated yet, then format it with the following folders:
  /// /, /Boot, /Applications.
  if (!diskFormatter.IsPartitionValid()) {
    BDiskFormatFactory<BootDeviceATA>::BFileDescriptor rootDesc{0};

    CopyMem(rootDesc.fFileName, "/", StrLen("/"));
    CopyMem(rootDesc.fForkName, kNewFSResourceFork, StrLen(kNewFSResourceFork));

    rootDesc.fBlobSz = BootDeviceATA::kSectorSize;
    rootDesc.fBlob = new Char[rootDesc.fBlobSz];

    SetMem(rootDesc.fBlob, 0, rootDesc.fBlobSz);
    CopyMem(rootDesc.fBlob, kMachineModel " startup disk.",
            strlen(kMachineModel " startup disk."));

    rootDesc.fKind = kNewFSCatalogKindDir;

    diskFormatter.Format(kMachineModel, &rootDesc, 1);
  }

  EFI::ExitBootServices(*MapKey, ImageHandle);

  hal_init_platform(handoverHdrPtr);

  EFI::Stop();

  CANT_REACH();
}
