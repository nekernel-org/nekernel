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

/// BUGS: 0

////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///  @name BFileReader class
///  @brief Reads the file as a blob.
///
///
////////////////////////////////////////////////////////////////////////////////////////////////////

/***
    @brief File Reader constructor.
*/
BFileReader::BFileReader(const CharacterType* path, EfiHandlePtr ImageHandle) {
  if (path != nullptr) {
    SizeT index = 0UL;
    for (; path[index] != L'\0'; ++index) {
      mPath[index] = path[index];
    }

    mPath[index] = 0;
  }

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

  mSizeFile = 0;
  mFile = kernelFile;
  mErrorCode = kNotSupported;
}

BFileReader::~BFileReader() {
  if (this->mFile) {
    this->mFile->Close(this->mFile);
    this->mFile = nullptr;
  }

  BSetMem(this->mPath, 0, kPathLen);
}

/**
    @brief this reads all of the buffer.
    @param ImageHandle used internally.
*/
Void BFileReader::Read() {
  /// Allocate Handover page.

  if (mBlob == nullptr) {
    UInt8* blob = (UInt8*)kHandoverStartKernel;

    if (BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1,
                          (EfiPhysicalAddress*)&blob) != kEfiOk) {
      EFI::RaiseHardError(L"HCoreLdr_PageError", L"Allocation error.");
    }

    mBlob = blob;
  }

  mErrorCode = kNotSupported;

  if (mFile->Read(mFile, &mSizeFile, (VoidPtr)((UIntPtr)mBlob)) != kEfiOk)
    return;

  mErrorCode = kOperationOkay;
}
