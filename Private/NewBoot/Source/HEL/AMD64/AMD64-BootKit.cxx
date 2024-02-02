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
    for (; path[index] != L'0'; ++index) {
      mPath[index] = path[index];
    }

    mPath[index] = 0;
  }
}

/**
@brief this reads all of the buffer.
*/
HCore::VoidPtr BFileReader::ReadAll() {
  BTextWriter writer;
  writer.WriteString(L"*** BFileReader::ReadAll: Reading ")
      .WriteString(mPath)
      .WriteString(L" *** \r\n");

  EfiFileDevicePathProtocol loadFile{0};
  loadFile.LengthData[0] = 0xFF;
  loadFile.LengthData[1] = 0;

  loadFile.Type = kEFIMediaDevicePath;
  loadFile.SubType = 0;  // from all drives.

  BCopyMem(loadFile.Path, mPath, kPathLen);

  return nullptr;
}
