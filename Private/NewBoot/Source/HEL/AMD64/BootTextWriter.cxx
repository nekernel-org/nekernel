/* -------------------------------------------

    Copyright Mahrouss Logic

    File: String.cxx
    Purpose: NewBoot string library

    Revision History:



------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

/// BUGS: 0

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@brief puts wrapper over EFI ConOut.
*/
BTextWriter &BTextWriter::Write(const CharacterTypeUTF16 *str) {
#ifdef __DEBUG__
  if (!str || *str == 0) return *this;

  ST->ConOut->OutputString(ST->ConOut, str);
#endif  // ifdef __DEBUG__

  return *this;
}

BTextWriter &BTextWriter::Write(const UChar *str) {
#ifdef __DEBUG__
  if (!str || *str == 0) return *this;

  CharacterTypeUTF16 strTmp[2];
  strTmp[1] = 0;

  for (size_t i = 0; str[i] != 0; i++) {
    strTmp[0] = str[i];
    ST->ConOut->OutputString(ST->ConOut, strTmp);
  }
#endif  // ifdef __DEBUG__

  return *this;
}

/**
@brief putc wrapper over EFI ConOut.
*/
BTextWriter &BTextWriter::WriteCharacter(CharacterTypeUTF16 c) {
#ifdef __DEBUG__
  EfiCharType str[2];

  str[0] = c;
  str[1] = 0;
  ST->ConOut->OutputString(ST->ConOut, str);
#endif  // ifdef __DEBUG__

  return *this;
}

BTextWriter &BTextWriter::Write(const Long &x) {
#ifdef __DEBUG__
  this->Write(L"0x");
  this->_Write(x);

#endif  // ifdef __DEBUG__

  return *this;
}

BTextWriter &BTextWriter::_Write(const Long &x) {
#ifdef __DEBUG__
  int y = x / 16;
  int h = x % 16;

  if (y) this->_Write(y);

  /* fail if the hex number is not base-16 */
  if (h > 15) {
    this->WriteCharacter('?');
    return *this;
  }

  if (y < 0) y = -y;

  const char NUMBERS[17] = "0123456789ABCDEF";

  this->WriteCharacter(NUMBERS[h]);
#endif  // ifdef __DEBUG__

  return *this;
}
