/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/Utils.hpp>

namespace HCore {
Int string_compare(const Char *src, const Char *cmp, Size size) {
  Int32 counter = 0;

  for (Size index = 0; index < size; ++index) {
    if (src[index] != cmp[index]) ++counter;
  }

  return counter;
}

void rt_zero_memory(voidPtr pointer, Size len) {
  rt_set_memory((voidPtr)pointer, 0, len);
}

Size string_length(const Char *str, SizeT _len) {
  if (*str == '\0') return 0;

  Size len{0};
  while (str[len] != '\0') {
    if (len > _len) {
      break;
    }

    ++len;
  }

  return len;
}

Size string_length(const Char *str) {
  if (*str == '\0') return 0;

  Size len{0};
  while (str[len] != '\0') ++len;

  return len;
}

voidPtr rt_set_memory(voidPtr src, char value, Size len) {
  if (!src || len < 1) return nullptr;
  char *start = reinterpret_cast<Char *>(src);

  while (len) {
    *start = value;
    ++start;
    --len;
  }

  return (voidPtr)start;
}

Int rt_move_memory(const voidPtr src, voidPtr dst, Size len) {
  if (len < 1) return -2;
  if (!src || !dst) return -1;

  char *srcChr = reinterpret_cast<Char *>(src);
  char *dstChar = reinterpret_cast<Char *>(dst);
  Size index = 0;

  while (index < len) {
    dstChar[index] = srcChr[index];
    srcChr[index] = 0;

    index++;
  }

  return 0;
}

Int rt_copy_memory(const voidPtr src, voidPtr dst, Size len) {
  if (len < 1) return -2;
  if (!src || !dst) return -1;

  char *srcChr = reinterpret_cast<char *>(src);
  char *dstChar = reinterpret_cast<char *>(dst);
  Size index = 0;

  while (index < len) {
    dstChar[index] = srcChr[index];
    index++;
  }

  return 0;
}

const Char *alloc_string(const Char *text) {
  if (!text) return nullptr;

  const Char *string = new Char[string_length(text)];
  if (!string) return nullptr;

  voidPtr vText = reinterpret_cast<voidPtr>(const_cast<char *>(text));
  voidPtr vStr = reinterpret_cast<voidPtr>(const_cast<char *>(string));
  rt_copy_memory(vText, vStr, string_length(text));
  return string;
}

Int to_uppercase(Int character) {
  if (character >= 'a' && character <= 'z') return character - 0x20;

  return character;
}

Int to_lower(Int character) {
  if (character >= 'A' && character <= 'Z') return character + 0x20;

  return character;
}

bool to_str(Char *str, Int limit, Int base) {
  if (limit == 0) return false;

  Int copy_limit = limit;
  Int cnt = 0;
  Int ret = base;

  while (limit != 1) {
    ret = ret % 10;
    str[cnt] = ret;

    ++cnt;
    --limit;
    --ret;
  }

  str[copy_limit] = '\0';
  return true;
}

Boolean is_space(Char chr) { return chr == ' '; }

Boolean is_newln(Char chr) { return chr == '\n'; }

voidPtr string_in_string(const char *in, const char *needle) {
  for (size_t i = 0; i < string_length(in); ++i) {
    if (string_compare(in + i, needle, string_length(needle)) == 0)
      return reinterpret_cast<voidPtr>(const_cast<char *>(in + i));
  }

  return nullptr;
}

// @brief Checks for a string start at the character.

char *string_from_char(char *str, const char chr) {
  while (*str != chr) {
    ++str;

    if (*str == 0) return nullptr;
  }

  return str;
}
}  // namespace HCore

extern "C" void memset(void *dst, char src, size_t len) {
  HCore::rt_set_memory(dst, src, len);
}

extern "C" void memcpy(void *dst, void *src, size_t len) {
  HCore::rt_copy_memory(src, dst, len);
}
