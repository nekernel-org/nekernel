/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/Utils.h>

namespace Kernel {
Size wrt_string_len(const Utf16Char* str) {
  SizeT len{0};

  while (str[len] != u'\0') ++len;

  return len;
}

Int wrt_copy_memory(const voidPtr src, voidPtr dst, Size len) {
  Utf16Char* srcChr  = reinterpret_cast<Utf16Char*>(src);
  Utf16Char* dstChar = reinterpret_cast<Utf16Char*>(dst);

  Size index = 0;

  while (index < len) {
    dstChar[index] = srcChr[index];
    ++index;
  }

  dstChar[index] = 0;

  return index;
}
}  // namespace Kernel