/* -------------------------------------------

    Copyright Mahrouss Logic

    File: FileReader.cxx
    Purpose: New Boot FileReader,
    Read complete file and store it in a buffer.

------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>
#include <FirmwareKit/Handover.hxx>
#include <cstddef>

/// @file BootFileReader
/// @brief Bootloader File reader.
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
BFileReader::BFileReader(const CharacterTypeUTF16* path,
                         EfiHandlePtr ImageHandle) {
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
    mWriter.Write(L"New Boot: Fetch-Protocol: No-Such-Protocol").Write(L"\r");
    this->mErrorCode = kNotSupported;
  }

  if (BS->HandleProtocol(img->DeviceHandle, &guidEfp, (void**)&efp) != kEfiOk) {
    mWriter.Write(L"New Boot: Fetch-Protocol: No-Such-Protocol").Write(L"\r");
    this->mErrorCode = kNotSupported;
    return;
  }

  /// Start doing disk I/O

  if (efp->OpenVolume(efp, &rootFs) != kEfiOk) {
    mWriter.Write(L"New Boot: Fetch-Protocol: No-Such-Volume").Write(L"\r");
    EFI::ThrowError(L"NoSuchVolume", L"No Such volume.");
    this->mErrorCode = kNotSupported;
    return;
  }

  EfiFileProtocol* kernelFile = nullptr;

  if (rootFs->Open(rootFs, &kernelFile, mPath, kEFIFileRead, kEFIReadOnly) !=
      kEfiOk) {
    mWriter.Write(L"New Boot: Fetch-Protocol: No-Such-Path: ")
        .Write(mPath)
        .Write(L"\r");
    EFI::ThrowError(L"NoSuchPath", L"No Such file on filesystem.");
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

  if (this->mBlob) BS->FreePool(mBlob);

  BSetMem(this->mPath, 0, kPathLen);
}

/**
    @brief this reads all of the buffer.
    @param until read until size is reached.
*/
Void BFileReader::ReadAll(SizeT until, SizeT chunk) {
  if (mBlob == nullptr) {
    if (auto err = BS->AllocatePool(EfiLoaderCode, until, (VoidPtr*)&mBlob) !=
                   kEfiOk) {
      mWriter.Write(L"*** EFI-Code: ").Write(err).Write(L" ***\r");
      EFI::ThrowError(L"OutOfMemory", L"Allocation error.");
    }
  }

  mErrorCode = kNotSupported;

  UInt64 bufSize = chunk;
  UInt64 szCnt = 0;
  UInt64 curSz = 0;

  while (szCnt < until) {
    if (mFile->Read(mFile, &bufSize, (VoidPtr)((UIntPtr)mBlob + curSz)) !=
        kEfiOk) {
      break;
    }

    szCnt += bufSize;
    curSz += bufSize;

    if (bufSize == 0) break;
  }

  mSizeFile = curSz;
  mErrorCode = kOperationOkay;
}

/// @brief error code getter.
/// @return the error code.
Int32& BFileReader::Error() { return mErrorCode; }

/// @brief blob getter.
/// @return the blob.
VoidPtr BFileReader::Blob() { return mBlob; }

/// @breif Size getter.
/// @return the size of the file.
UInt64& BFileReader::Size() { return mSizeFile; }
