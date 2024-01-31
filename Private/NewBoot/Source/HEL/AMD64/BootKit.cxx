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

/// bugs 0

HCore::SizeT BStrLen(const char *ptr) {
  long long int cnt = 0;

  while (*ptr != 0) {
    ++ptr;
    ++cnt;
  }

  return cnt;
}

HCore::SizeT BSetMem(char *src, const char byte, const HCore::SizeT len) {
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
