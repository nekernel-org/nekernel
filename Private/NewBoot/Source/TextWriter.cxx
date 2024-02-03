/* -------------------------------------------

    Copyright Mahrouss Logic

    File: String.cxx
    Purpose: NewBoot string library

    Revision History:



------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <FSKit/NewFS.hxx>

/// bugs 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@brief puts wrapper over EFI ConOut.
*/
BTextWriter &BTextWriter::WriteString(const CharacterType *str) {
#ifdef __DEBUG__

  if (*str == 0 || !str) return *this;

  ST->ConOut->OutputString(ST->ConOut, str);
#endif

  return *this;
}

/**
@brief putc wrapper over EFI ConOut.
*/
BTextWriter &BTextWriter::WriteCharacter(CharacterType c) {
#ifdef __DEBUG__
  EfiCharType str[2];
  str[0] = c;
  str[1] = 0;
  ST->ConOut->OutputString(ST->ConOut, str);
#endif

  return *this;
}
