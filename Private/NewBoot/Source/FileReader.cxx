/* -------------------------------------------

    Copyright Mahrouss Logic

    File: FileReader.cxx
    Purpose: NewBoot FileReader,
    Read complete file and store it in a buffer.

    Revision History:



------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <FSKit/NewFS.hxx>

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
BFileReader::BFileReader(const CharacterType *path) {
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
HCore::VoidPtr BFileReader::Fetch(SizeT &size) {
  mWriter.WriteString(L"HCoreLdr: Fetch-File: ")
      .WriteString(mPath)
      .WriteString(L"\r\n");

  this->mCached = true;
  this->mErrorCode = kNotSupported;

  return nullptr;
}
