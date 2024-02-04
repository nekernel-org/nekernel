/* -------------------------------------------

    Copyright Mahrouss Logic

    File: FileReader.cxx
    Purpose: NewBoot FileReader,
    Read complete file and store it in a buffer.

    Revision History:



------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <EFIKit/Handover.hxx>

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//  @brief BImageReader class
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/***
    @brief File Reader constructor.
*/
BFileReader::BFileReader(const CharacterType* path) {
  if (path != nullptr) {
    SizeT index = 0UL;
    for (; path[index] != L'\0'; ++index) {
      mPath[index] = path[index];
    }

    mPath[index] = 0;
  }
}

BFileReader::~BFileReader() {
  if (this->mBlob) {
    BS->FreePool(this->mBlob);
  }
}

/**
    @brief this reads all of the buffer.
    @param ImageHandle used internally.
*/
Void BFileReader::Fetch(EfiHandlePtr ImageHandle) {
  mWriter.WriteString(L"HCoreLdr: Fetch-File: ")
      .WriteString(mPath)
      .WriteString(L"\r\n");

  /// Load protocols with their GUIDs.

  EfiGUID guidEfp = EfiGUID(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID);

  EfiSimpleFilesystemProtocol* efp = nullptr;
  EfiFileProtocol* rootFs = nullptr;

  EfiLoadImageProtocol* img = nullptr;
  EfiGUID guidImg = EfiGUID(EFI_LOADED_IMAGE_PROTOCOL_GUID);

  if (BS->HandleProtocol(ImageHandle, &guidImg, (void**)&img) != kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Protocol")
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
  }

  if (BS->HandleProtocol(img->DeviceHandle, &guidEfp, (void**)&efp) != kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Protocol")
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  /// Start doing disk I/O

  if (efp->OpenVolume(efp, &rootFs) != kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Volume")
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  /// Open kernel.

  EfiFileProtocol* kernelFile;

  if (rootFs->Open(rootFs, &kernelFile, mPath, kEFIFileRead,
                   kEFIReadOnly | kEFIHidden | kEFISystem) != kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Path: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  if (kernelFile->Revision < EFI_FILE_PROTOCOL_REVISION2) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: Invalid-Revision: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");
    return;
  }

  /// File FAT info.

  UInt32 szInfo = sizeof(EfiFileInfo);
  EfiFileInfo info{0};

  guidEfp = EfiGUID(EFI_FILE_INFO_GUID);

  if (kernelFile->GetInfo(kernelFile, &guidEfp, &szInfo, (void*)&info) !=
      kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Path: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  mWriter.WriteString(L"HCoreLdr: Fetch-Info: In-Progress...")
      .WriteString(L"\r\n");

  VoidPtr blob = (VoidPtr)kHandoverStartKernel;

  mWriter.WriteString(L"HCoreLdr: Fetch-Info: OK...").WriteString(L"\r\n");

  UInt32 sz = info.FileSize;

  BSetMem((CharacterType*)blob, 0, sz);

  auto resultEfiRead = kernelFile->Read(kernelFile, &sz, blob);
  kernelFile->Close(kernelFile);

  if (resultEfiRead != kEfiOk) return;

  mCached = true;
  mErrorCode = kOperationOkay;

  mBlob = blob;

  // We are done!

  mWriter.WriteString(L"HCoreLdr: Fetch: OK.").WriteString(L"\r\n");
}
