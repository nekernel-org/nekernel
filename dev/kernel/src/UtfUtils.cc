/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/Utils.h>

namespace Kernel {
Size urt_string_len(const Utf8Char* str) {
  SizeT len{0};

  while (str[len] != u'\0') ++len;

  return len;
}

Int urt_copy_memory(const voidPtr src, voidPtr dst, Size len) {
  Utf8Char* srcChr  = reinterpret_cast<Utf8Char*>(src);
  Utf8Char* dstChar = reinterpret_cast<Utf8Char*>(dst);

  Size index = 0;

  while (index < len) {
    dstChar[index] = srcChr[index];
    ++index;
  }

  dstChar[index] = 0;

  return index;
}
}  // namespace Kernel