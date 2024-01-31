/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>
#include <EFIKit/EFILib.hxx>

#include "NewKit/Defines.hpp"

/// bugs 0

HCore::SizeT BStrLen(const CharacterType *ptr) {
  HCore::SizeT cnt = 0;

  while (*ptr != (CharacterType)0) {
    ++ptr;
    ++cnt;
  }

  return cnt;
}

/**
    @biref set memory
*/

HCore::SizeT BSetMem(CharacterType *src, const CharacterType byte,
                     const HCore::SizeT len) {
  HCore::SizeT cnt = 0UL;

  while (*src != 0) {
    if (cnt > len) break;

    *src = byte;
    ++src;

    ++cnt;
  }

  return cnt;
}

/**
@brief puts wrapper over VGA.
*/
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
  writer.WriteString(L"*** PE/COFF: Reading ")
      .WriteString(mPath)
      .WriteString(L" *** \r\n");

  return nullptr;
}
