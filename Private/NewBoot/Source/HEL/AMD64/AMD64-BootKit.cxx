/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <FSKit/NewFS.hxx>

#include "EFIKit/EFI.hxx"
#include "NewKit/Macros.hpp"

/// bugs 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

HCore::SizeT BCopyMem(CharacterType *dest, CharacterType *src,
                      const HCore::SizeT len) {
  if (!dest || !src) return 0;

  SizeT index = 0UL;
  for (; index < len; ++index) {
    dest[index] = src[index];
  }

  return index;
}

HCore::SizeT BStrLen(const CharacterType *ptr) {
  if (!ptr) return 0;

  HCore::SizeT cnt = 0;

  while (*ptr != (CharacterType)0) {
    ++ptr;
    ++cnt;
  }

  return cnt;
}

HCore::SizeT BSetMem(CharacterType *src, const CharacterType byte,
                     const HCore::SizeT len) {
  if (!src) return 0;

  HCore::SizeT cnt = 0UL;

  while (*src != 0) {
    if (cnt > len) break;

    *src = byte;
    ++src;

    ++cnt;
  }

  return cnt;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

BTextWriter &BTextWriter::WriteString(const CharacterType *str) {
  if (*str == 0 || !str) return *this;

  ST->ConOut->OutputString(ST->ConOut, str);

  return *this;
}

/**
@brief putc wrapper over VGA.
*/
BTextWriter &BTextWriter::WriteCharacter(CharacterType c) {
  EfiCharType str[2];
  str[0] = c;
  str[1] = 0;
  ST->ConOut->OutputString(ST->ConOut, str);

  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
HCore::VoidPtr BFileReader::ReadAll(SizeT &size) {
  BTextWriter writer;
  writer.WriteString(L"*** BFileReader::ReadAll: Reading ")
      .WriteString(mPath)
      .WriteString(L" *** \r\n");

  EfiHandlePtr handleFile = nullptr;
  EfiLoadFileProtocol *loadFile = nullptr;

  EfiGUID loadFileGUID = EfiGUID(EFI_LOAD_FILE_PROTOCOL_GUID);

  BS->LocateProtocol(&loadFileGUID, nullptr, (VoidPtr *)&loadFile);

  if (loadFile) {
    writer.WriteString(L"HCoreLdr: Loading: ")
        .WriteString(mPath)
        .WriteString(L"\r\n");

    UInt32 *bufSz = nullptr;
    VoidPtr buf = nullptr;

    BS->AllocatePool(EfiLoaderCode, sizeof(UInt32), (VoidPtr *)&bufSz);
    *bufSz = 0;

    BS->AllocatePool(EfiLoaderCode, *bufSz, &buf);

    if (!buf) return nullptr;

    EfiFileDevicePathProtocol filePath{0};

    filePath.Proto.Type = kEFIMediaDevicePath;
    filePath.Proto.SubType = kEFIMediaDevicePath;  // from all drives.

    BCopyMem(filePath.Path, mPath, kPathLen);

    auto err = loadFile->LoadFile(loadFile, &filePath, true, bufSz, buf);

    size = *bufSz;

    if (err == kEfiOk) {
      writer.WriteString(L"HCoreLdr: Loaded: ")
          .WriteString(mPath)
          .WriteString(L"\r\n");
    } else {
      BS->FreePool(buf);
      buf = nullptr;

      switch (err) {
        case 2: {
          writer.WriteString(L"HCoreLdr: Error: ")
              .WriteString(mPath)
              .WriteString(L", Code: Invalid-Parameter")
              .WriteString(L"\r\n");

          break;
        }
        case 14: {
          writer.WriteString(L"HCoreLdr: Error: ")
              .WriteString(mPath)
              .WriteString(L" , EFI-Code: Not-Found")
              .WriteString(L"\r\n");

          break;
        }
      }
    }

    BS->FreePool(bufSz);
    bufSz = nullptr;

    return buf;
  }

  return nullptr;
}
