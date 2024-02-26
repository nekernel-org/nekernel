/* -------------------------------------------

    Copyright Mahrouss Logic

    File: String.cxx
    Purpose: NewBoot string library

    Revision History:



------------------------------------------- */

#include <BootKit/BootKit.hxx>

/// BUGS: 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@brief puts wrapper over EFI ConOut.
*/
BTextWriter &BTextWriter::Write(const CharacterType *str) {
  if (*str == 0 || !str) return *this;

  ST->ConOut->OutputString(ST->ConOut, str);

  return *this;
}

/**
@brief putc wrapper over EFI ConOut.
*/
BTextWriter &BTextWriter::WriteCharacter(CharacterType c) {
  EfiCharType str[2];
  str[0] = c;
  str[1] = 0;
  ST->ConOut->OutputString(ST->ConOut, str);

  return *this;
}

BTextWriter &BTextWriter::Write(const Long &x) {
  int y = x / 16;
  int h = x % 16;

  if (y) this->Write(y);

  /* fail if the hex number is not base-16 */
  if (h > 15) {
    this->WriteCharacter('?');
    return *this;
  }

  if (y < 0) y = -y;

  const char NUMBERS[17] = "0123456789ABCDEF";

  this->WriteCharacter(NUMBERS[h]);
  return *this;
}
