// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#include <NeKit/Utils.h>

namespace Ne::Kernel {
Int32 rt_string_cmp(const Char* src, const Char* cmp, Size size) {
  for (Size i{}; i < size; ++i) {
    if (src[i] != cmp[i]) return static_cast<Int32>(src[i]) - static_cast<Int32>(cmp[i]);
  }
  return 0;
}

SizeT rt_string_len(const Char* str, SizeT max_len) {
  SizeT len{};
  while (len < max_len && str[len] != '\0') ++len;
  return len;
}

Size rt_string_len(const Char* ptr) {
  Size cnt{};
  while (ptr[cnt] != '\0') ++cnt;
  return cnt;
}

const Char* rt_alloc_string(const Char* src) {
  SizeT slen   = rt_string_len(src);
  Char* buffer = new Char[slen + 1];

  if (!buffer) return nullptr;

  rt_set_memory(buffer, 0, slen);

  if (rt_copy_memory_safe(reinterpret_cast<voidPtr>(const_cast<Char*>(src)),
                          reinterpret_cast<voidPtr>(buffer), slen, slen + 1) < 0) {
    delete[] buffer;
    return nullptr;
  }

  buffer[slen] = '\0';
  return buffer;
}

Int32 rt_copy_memory_safe(const voidPtr src, voidPtr dst, Size len, Size dst_size) {
  if (!src || !dst || len > dst_size) {
    if (dst && dst_size) {
      rt_set_memory_safe(dst, 0, dst_size, dst_size);
    }
    return -1;
  }

  auto s = reinterpret_cast<const UInt8*>(src);
  auto d = reinterpret_cast<UInt8*>(dst);

  for (Size i{}; i < len; ++i) d[i] = s[i];

  return static_cast<Int>(len);
}

voidPtr rt_set_memory_safe(voidPtr dst, UInt32 value, Size len, Size dst_size) {
  if (!dst || len > dst_size) return nullptr;
  auto  p = reinterpret_cast<UInt8*>(dst);
  UInt8 v = static_cast<UInt8>(value & 0xFF);
  for (Size i{}; i < len; ++i) p[i] = v;
  return dst;
}

Void rt_zero_memory(voidPtr pointer, Size len) {
  rt_set_memory_safe(pointer, 0, len, len);
}

#ifdef __NE_ENFORCE_DEPRECATED_WARNINGS
[[deprecated("Use rt_set_memory_safe instead")]]
#endif
voidPtr rt_set_memory(voidPtr src, UInt32 value, Size len) {
  if (!src) return nullptr;
  auto  p = reinterpret_cast<UInt8*>(src);
  UInt8 v = static_cast<UInt8>(value & 0xFF);
  for (Size i{}; i < len; ++i) p[i] = v;
  return src;
}

#ifdef __NE_ENFORCE_DEPRECATED_WARNINGS
[[deprecated("Use rt_copy_memory_safe instead")]]
#endif
Int32 rt_copy_memory(const voidPtr src, voidPtr dst, Size len) {
  if (!len) return 0;
  if (!src || !dst) return -1;
  
  auto s = reinterpret_cast<const UInt8*>(src);
  auto d = reinterpret_cast<UInt8*>(dst);

  for (Size i{}; i < len; ++i) d[i] = s[i];

  return static_cast<Int>(len);
}

Int32 rt_to_uppercase(Int ch) {
  return (ch >= 'a' && ch <= 'z') ? ch - 0x20 : ch;
}

Int32 rt_to_lower(Int ch) {
  return (ch >= 'A' && ch <= 'Z') ? ch + 0x20 : ch;
}

Int32 rt_is_alnum(Int ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9');
}

Boolean rt_is_space(Int ch) {
  return ch == ' ';
}

Boolean rt_is_newln(Int ch) {
  return ch == '\n';
}

Char rt_to_char(UInt64 value, Int32 base) {
  static constexpr Char kDigits[] = "0123456789ABCDEF";
  return kDigits[value % base];
}

Bool rt_to_string(Char* str, UInt64 value, Int32 base) {
  if (!str || base < 2 || base > 16) return NO;

  Int i{};
  do {
    str[i++] = rt_to_char(value, base);
    value /= base;
  } while (value);
  str[i] = '\0';
  // in-place
  for (Int j{}; j < i / 2; ++j) {
    Char tmp       = str[j];
    str[j]         = str[i - j - 1];
    str[i - j - 1] = tmp;
  }

  return YES;
}

VoidPtr rt_string_in_string(const Char* haystack, const Char* needle) {
  if (!haystack || !needle) return nullptr;

  SizeT needle_len = rt_string_len(needle);
  SizeT hay_len    = rt_string_len(haystack);

  if (needle_len > hay_len) return nullptr;
  for (SizeT i{}; i <= hay_len - needle_len; ++i) {
    if (rt_string_cmp(haystack + i, needle, needle_len) == 0) {
      return reinterpret_cast<voidPtr>(const_cast<Char*>(haystack + i));
    }
  }
  return nullptr;
}

Char* rt_string_has_char(Char* str, Char ch) {
  if (!str) return nullptr;
  while (*str && *str != ch) ++str;
  return (*str == ch) ? str : nullptr;
}
}  // namespace Ne::Kernel
