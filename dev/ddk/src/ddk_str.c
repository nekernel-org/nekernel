/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK Strings.

------------------------------------------- */

#include <DDKKit/str.h>

DDK_EXTERN size_t kstrlen(const char* in) {
  if (in == nil) return 0;

  if (*in == 0) return 0;

  size_t index = 0;

  while (in[index] != 0) {
    ++index;
  }

  return index;
}

DDK_EXTERN int kstrncpy(char* dst, const char* src, size_t len) {
  size_t index = 0;

  while (index != len) {
    dst[index] = src[index];
    ++index;
  }

  return index;
}
