/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NeKit/Utils.h>

/// @author Amlal El Mahrouss (amlal@nekernel.org)

namespace Kernel {
Size urt_string_len(const Utf8Char* str) {
  SizeT len{0};

  while (str[len] != u8'\0') ++len;

  return len;
}

Void urt_set_memory(const voidPtr src, UInt32 dst, Size len) {
  Utf8Char* srcChr = reinterpret_cast<Utf8Char*>(src);
  Size      index  = 0;

  while (index < len) {
    srcChr[index] = dst;
    ++index;
  }
}

Int32 rt_string_cmp(const Utf8Char* src, const Utf8Char* cmp, Size size) {
  Int32 counter = 0;

  for (Size index = 0; index < size; ++index) {
    if (src[index] != cmp[index]) ++counter;
  }

  return counter;
}

Int urt_copy_memory(const VoidPtr src, VoidPtr dst, Size len) {
  Utf8Char* srcChr  = reinterpret_cast<Utf8Char*>(src);
  Utf8Char* dstChar = reinterpret_cast<Utf8Char*>(dst);

  Size index = 0;

  while (index < len) {
    dstChar[index] = srcChr[index];
    ++index;
  }

  return index;
}
}  // namespace Kernel
