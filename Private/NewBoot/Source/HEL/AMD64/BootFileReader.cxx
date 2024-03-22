/* -------------------------------------------

    Copyright Mahrouss Logic

    File: FileReader.cxx
    Purpose: NewBoot FileReader,
    Read complete file and store it in a buffer.

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <FirmwareKit/Handover.hxx>

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
BFileReader::BFileReader(const CharacterTypeUTF16* path, EfiHandlePtr ImageHandle) {
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
    mWriter.Write(L"NewBoot: Fetch-Protocol: No-Such-Protocol").Write(L"\r\n");
    this->mErrorCode = kNotSupported;
  }

  if (BS->HandleProtocol(img->DeviceHandle, &guidEfp, (void**)&efp) != kEfiOk) {
    mWriter.Write(L"NewBoot: Fetch-Protocol: No-Such-Protocol").Write(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  /// Start doing disk I/O

  if (efp->OpenVolume(efp, &rootFs) != kEfiOk) {
    mWriter.Write(L"NewBoot: Fetch-Protocol: No-Such-Volume").Write(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  EfiFileProtocol* kernelFile = nullptr;

  if (rootFs->Open(rootFs, &kernelFile, mPath, kEFIFileRead, kEFIReadOnly) !=
      kEfiOk) {
    mWriter.Write(L"NewBoot: Fetch-Protocol: No-Such-Path: ")
        .Write(mPath)
        .Write(L"\r\n");
    this->mErrorCode = kNotSupported;
    return;
  }

  rootFs->Close(rootFs);

  mSizeFile = 0;
  mFile = kernelFile;
  mErrorCode = kOperationOkay;
}

BFileReader::~BFileReader() {
  if (this->mFile) {
    this->mFile->Close(this->mFile);
    this->mFile = nullptr;
  }

  if (this->mBlob)
    BS->FreePool(mBlob);

  BSetMem(this->mPath, 0, kPathLen);
}

/**
    @brief this reads all of the buffer.
    @param ImageHandle used internally.
*/
Void BFileReader::ReadAll() {
  /// Allocate Handover page.

  if (this->mErrorCode != kOperationOkay) return;

  if (mBlob == nullptr) {
    if (auto err = BS->AllocatePool(EfiLoaderCode, mSizeFile,
                          (VoidPtr*)&mBlob) != kEfiOk) {
      mWriter.Write(L"*** EFI-Code: ").Write(err).Write(L" ***\r\n");
      EFI::RaiseHardError(L"NewBoot_PageError", L"Allocation error.");
    }
  }

  mErrorCode = kNotSupported;

  if (mFile->Read(mFile, &mSizeFile, (VoidPtr)((UIntPtr)mBlob)) != kEfiOk)
    return;

  mErrorCode = kOperationOkay;
}
