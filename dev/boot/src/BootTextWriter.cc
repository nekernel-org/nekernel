/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: BootTextWriter.cc
  Purpose: BootZ string library

  Revision History:



------------------------------------------- */

#include <BootKit/BootKit.h>
#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>
#include <FirmwareKit/EFI/API.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BUGS: 0																							  ///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@brief puts wrapper over EFI ConOut.
*/
Boot::BootTextWriter& Boot::BootTextWriter::Write(const CharacterTypeUTF16* str) {
  NE_UNUSED(str);

#ifdef __DEBUG__
  if (!str || *str == 0) return *this;

  CharacterTypeUTF16 strTmp[2];
  strTmp[1] = 0;

  for (size_t i = 0; str[i] != 0; i++) {
    if (str[i] == '\r') {
      strTmp[0] = str[i];
      ST->ConOut->OutputString(ST->ConOut, strTmp);

      strTmp[0] = '\n';
      ST->ConOut->OutputString(ST->ConOut, strTmp);
    } else {
      strTmp[0] = str[i];
      ST->ConOut->OutputString(ST->ConOut, strTmp);
    }
  }
#endif  // ifdef __DEBUG__

  return *this;
}

/// @brief UTF-8 equivalent of Write (UTF-16).
/// @param str the input string.
Boot::BootTextWriter& Boot::BootTextWriter::Write(const Char* str) {
  NE_UNUSED(str);

#ifdef __DEBUG__
  if (!str || *str == 0) return *this;

  CharacterTypeUTF16 strTmp[2];
  strTmp[1] = 0;

  for (size_t i = 0; str[i] != 0; i++) {
    if (str[i] == '\r') {
      strTmp[0] = str[i];
      ST->ConOut->OutputString(ST->ConOut, strTmp);

      strTmp[0] = '\n';
      ST->ConOut->OutputString(ST->ConOut, strTmp);
    } else {
      strTmp[0] = str[i];
      ST->ConOut->OutputString(ST->ConOut, strTmp);
    }
  }
#endif  // ifdef __DEBUG__

  return *this;
}

Boot::BootTextWriter& Boot::BootTextWriter::Write(const UChar* str) {
  NE_UNUSED(str);

#ifdef __DEBUG__
  if (!str || *str == 0) return *this;

  CharacterTypeUTF16 strTmp[2];
  strTmp[1] = 0;

  for (size_t i = 0; str[i] != 0; i++) {
    if (str[i] == '\r') {
      strTmp[0] = str[i];
      ST->ConOut->OutputString(ST->ConOut, strTmp);

      strTmp[0] = '\n';
      ST->ConOut->OutputString(ST->ConOut, strTmp);
    } else {
      strTmp[0] = str[i];
      ST->ConOut->OutputString(ST->ConOut, strTmp);
    }
  }
#endif  // ifdef __DEBUG__

  return *this;
}

/**
@brief putc wrapper over EFI ConOut.
*/
Boot::BootTextWriter& Boot::BootTextWriter::WriteCharacter(CharacterTypeUTF16 c) {
  NE_UNUSED(c);

#ifdef __DEBUG__
  EfiCharType str[2];

  str[0] = c;
  str[1] = 0;
  ST->ConOut->OutputString(ST->ConOut, str);
#endif  // ifdef __DEBUG__

  return *this;
}

Boot::BootTextWriter& Boot::BootTextWriter::Write(const UInt64& x) {
  NE_UNUSED(x);

#ifdef __DEBUG__
  this->_Write(x);
  this->Write("h");
#endif  // ifdef __DEBUG__

  return *this;
}

Boot::BootTextWriter& Boot::BootTextWriter::_Write(const UInt64& x) {
  NE_UNUSED(x);

#ifdef __DEBUG__
  UInt64 y = (x > 0 ? x : -x) / 16;
  UInt64 h = (x > 0 ? x : -x) % 16;

  if (y) this->_Write(y);

  /* fail if the hex number is not base-16 */
  if (h > 16) {
    this->WriteCharacter('?');
    return *this;
  }

  if (y == ~0UL) y = -y;

  const char kNumberList[] = "0123456789ABCDEF";

  this->WriteCharacter(kNumberList[h]);
#endif  // ifdef __DEBUG__

  return *this;
}
