/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/Utils.h>

namespace Kernel {
Size wrt_string_len(const Utf16Char* str) {
  SizeT len{0};

  while (str[len] != 0) ++len;

  return len;
}
}  // namespace Kernel