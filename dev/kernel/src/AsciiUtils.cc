/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NeKit/Utils.h>

namespace Kernel {

Int     rt_copy_memory_safe(const voidPtr src, voidPtr dst, Size len, Size dst_size);
voidPtr rt_set_memory_safe(voidPtr dst, UInt32 value, Size len, Size dst_size);

Int32 rt_string_cmp(const Char* src, const Char* cmp, Size size) {
  for (Size i = 0; i < size; ++i) {
    if (src[i] != cmp[i]) return static_cast<Int32>(src[i]) - static_cast<Int32>(cmp[i]);
  }
  return 0;
}

SizeT rt_string_len(const Char* str, SizeT max_len) {
  SizeT len = 0;
  while (len < max_len && str[len] != '\0') ++len;
  return len;
}

Size rt_string_len(const Char* ptr) {
  Size cnt = 0;
  while (ptr[cnt] != '\0') ++cnt;
  return cnt;
}

const Char* rt_alloc_string(const Char* src) {
  SizeT slen   = rt_string_len(src);
  Char* buffer = new Char[slen + 1];
  if (!buffer) return nullptr;

  if (rt_copy_memory_safe(reinterpret_cast<voidPtr>(const_cast<Char*>(src)),
                          reinterpret_cast<voidPtr>(buffer), slen, slen + 1) < 0) {
    delete[] buffer;
    return nullptr;
  }

  buffer[slen] = '\0';
  return buffer;
}

Int rt_copy_memory_safe(const voidPtr src, voidPtr dst, Size len, Size dst_size) {
  if (!src || !dst || len > dst_size) {
    if (dst && dst_size) {
      rt_set_memory_safe(dst, 0, dst_size, dst_size);
    }
    return -1;
  }
  auto s = reinterpret_cast<const UInt8*>(src);
  auto d = reinterpret_cast<UInt8*>(dst);
  for (Size i = 0; i < len; ++i) d[i] = s[i];
  return static_cast<Int>(len);
}

voidPtr rt_set_memory_safe(voidPtr dst, UInt32 value, Size len, Size dst_size) {
  if (!dst || len > dst_size) return nullptr;
  auto  p = reinterpret_cast<UInt8*>(dst);
  UInt8 v = static_cast<UInt8>(value & 0xFF);
  for (Size i = 0; i < len; ++i) p[i] = v;
  return dst;
}

Void rt_zero_memory(voidPtr pointer, Size len) {
  rt_set_memory_safe(pointer, 0, len, len);
}

#ifdef __NE_ENFORCE_DEPRECATED_WARNINGS
[[deprecated("Use rt_set_memory_safe instead")]]
#endif
voidPtr
rt_set_memory(voidPtr src, UInt32 value, Size len) {
  if (!src) return nullptr;
  auto  p = reinterpret_cast<UInt8*>(src);
  UInt8 v = static_cast<UInt8>(value & 0xFF);
  for (Size i = 0; i < len; ++i) p[i] = v;
  return src;
}

#ifdef __NE_ENFORCE_DEPRECATED_WARNINGS
[[deprecated("Use rt_copy_memory_safe instead")]]
#endif
Int rt_copy_memory(const voidPtr src, voidPtr dst, Size len) {
  if (!src || !dst) return -1;
  auto s = reinterpret_cast<const UInt8*>(src);
  auto d = reinterpret_cast<UInt8*>(dst);

  for (Size i = 0; i < len; ++i) d[i] = s[i];

  return static_cast<Int>(len);
}

Int32 rt_to_uppercase(Int32 ch) {
  return (ch >= 'a' && ch <= 'z') ? ch - 0x20 : ch;
}

Int32 rt_to_lower(Int32 ch) {
  return (ch >= 'A' && ch <= 'Z') ? ch + 0x20 : ch;
}

Int32 rt_is_alnum(Int32 ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9');
}

Boolean rt_is_space(Char ch) {
  return ch == ' ';
}

Boolean rt_is_newln(Char ch) {
  return ch == '\n';
}

Char rt_to_char(UInt64 value, Int32 base) {
  static constexpr Char kDigits[] = "0123456789ABCDEF";
  return kDigits[value % base];
}

Bool rt_to_string(Char* str, UInt64 value, Int32 base) {
#ifdef __NE_AMD64__
  Int i = 0;
  do {
    str[i++] = rt_to_char(value, base);
    value /= base;
  } while (value);
  str[i] = '\0';
  // in-place
  for (Int j = 0; j < i / 2; ++j) {
    Char tmp       = str[j];
    str[j]         = str[i - j - 1];
    str[i - j - 1] = tmp;
  }
#endif
  return true;
}

VoidPtr rt_string_in_string(const Char* haystack, const Char* needle) {
  SizeT needle_len = rt_string_len(needle);
  SizeT hay_len    = rt_string_len(haystack);

  if (needle_len > hay_len) return nullptr;
  for (SizeT i = 0; i <= hay_len - needle_len; ++i) {
    if (rt_string_cmp(haystack + i, needle, needle_len) == 0) {
      return reinterpret_cast<voidPtr>(const_cast<Char*>(haystack + i));
    }
  }
  return nullptr;
}

Char* rt_string_has_char(Char* str, Char ch) {
  while (*str && *str != ch) ++str;
  return (*str == ch) ? str : nullptr;
}

EXTERN_C void* memset(void* dst, int c, long long unsigned int len) {
  return Kernel::rt_set_memory_safe(dst, c, static_cast<Size>(len), static_cast<Size>(len));
}

EXTERN_C void* memcpy(void* dst, const void* src, long long unsigned int len) {
  Kernel::rt_copy_memory_safe(const_cast<void*>(src), dst, static_cast<Size>(len),
                              static_cast<Size>(len));
  return dst;
}

EXTERN_C Kernel::Int32 strcmp(const char* a, const char* b) {
  return Kernel::rt_string_cmp(a, b, rt_string_len(a));
}

}  // namespace Kernel
