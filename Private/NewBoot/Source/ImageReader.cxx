/* -------------------------------------------

    Copyright Mahrouss Logic

    File: String.cxx
    Purpose: NewBoot string library

    Revision History:



------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <FSKit/NewFS.hxx>

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
BImageReader::BImageReader(const CharacterType *path) {
  if (path != nullptr) {
    SizeT index = 0UL;
    for (; path[index] != L'\0'; ++index) {
      mPath[index] = path[index];
    }

    mPath[index] = 0;
  }
}

/**
    @brief this reads all of the buffer.
    @param size, new buffer size.
*/
HCore::VoidPtr BImageReader::Fetch(SizeT &size) {
  mWriter.WriteString(L"HCoreLdr: Fetch-Image: ")
      .WriteString(mPath)
      .WriteString(L"\r\n");

  CharacterType bootBlockBytes[kATASectorSz] = {0};

  mDevice.Leak().mBase = kNewFSAddressAsLba;
  mDevice.Leak().mMaster = true;

  mDevice.Read(bootBlockBytes, kATASectorSz);

  NewBootBlock *bootBlock = (NewBootBlock *)bootBlockBytes;

  for (SizeT index = 0UL; index < kNewFSIdentLen; ++index) {
    if (bootBlock->Ident[index] != kNewFSIdent[index]) {
      mWriter.WriteString(L"HCoreLdr: NewFS: Not found.\r\n");
      return nullptr;
    }
  }

  /// get file catalog with mPath inside it.

  this->mCached = true;

  return nullptr;
}
