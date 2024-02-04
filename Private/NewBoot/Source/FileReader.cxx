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

#include "EFIKit/EFI.hxx"
#include "KernelKit/PE.hpp"
#include "NewKit/Defines.hpp"

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

  EfiFileProtocol* kernelFile = nullptr;

  if (rootFs->Open(rootFs, &kernelFile, mPath, kEFIFileRead, kEFIReadOnly) !=
      kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Path: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  rootFs->Close(rootFs);

  if (kernelFile->Revision < EFI_FILE_PROTOCOL_REVISION2) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: Invalid-Revision: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");
    return;
  }

  /// File FAT info.

  UInt32 szInfo = sizeof(EfiFileInfo);
  EfiFileInfo* info = nullptr;

  BS->AllocatePool(EfiLoaderData, szInfo, (void**)&info);

  guidEfp = EfiGUID(EFI_FILE_INFO_GUID);

  if (kernelFile->GetInfo(kernelFile, &guidEfp, &szInfo, info) != kEfiOk) {
    mWriter.WriteString(L"HCoreLdr: Fetch-Protocol: No-Such-Path: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  mWriter.WriteString(L"HCoreLdr: Fetch-Info: In-Progress...")
      .WriteString(L"\r\n");

  VoidPtr blob = (VoidPtr)kHandoverStartKernel;

  if (BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1,
                        (EfiPhysicalAddress*)&blob) != kEfiOk) {
    EFI::RaiseHardError(L"HCoreLdr_PageError", L"Allocation error.");
  }

  UInt32* sz = nullptr;

  BS->AllocatePool(EfiLoaderData, sizeof(UInt32), (VoidPtr*)&sz);

  *sz = info->FileSize;

  mWriter.WriteString(L"HCoreLdr: Fetch-Info: Read...").WriteString(L"\r\n");

  kernelFile->Read(kernelFile, sz, blob);

  mWriter.WriteString(L"HCoreLdr: Fetch-Info: Success...").WriteString(L"\r\n");

  mCached = true;
  mErrorCode = kOperationOkay;
  mBlob = blob;

  // We are done!
}
