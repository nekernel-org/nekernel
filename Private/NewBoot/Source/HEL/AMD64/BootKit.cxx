/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>
#include <EFIKit/EFILib.hxx>

#include "EFIKit/EFI.hxx"

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
void BTextWriter::WriteString(const CharacterType *str) {
  if (*str == 0 || !str) return;

  ST->ConOut->OutputString(ST->ConOut, str);
}

/**
@brief putc wrapper over VGA.
*/
void BTextWriter::WriteCharacter(CharacterType c) {
  EfiCharType str[2];
  str[0] = c;
  str[1] = 0;
  ST->ConOut->OutputString(ST->ConOut, str);
}
