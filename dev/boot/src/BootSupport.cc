/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/BootKit.h>
#include <BootKit/Support.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/EFI/EFI.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>

#ifdef __BOOTZ_STANDALONE__

/// @brief memset definition in C++.
/// @param dst destination pointer.
/// @param byte value to fill in.
/// @param len length of of src.
EXTERN_C VoidPtr memnset(void* dst, int byte, long long unsigned int len,
                         long long unsigned int dst_size) {
  if (!dst || len > dst_size) {
    // For now, we return nullptr or an error status.
    return nullptr;
  }
  unsigned char* p   = (unsigned char*) dst;
  unsigned char  val = (unsigned char) byte;
  for (size_t i = 0UL; i < len; ++i) {
    p[i] = val;
  }
  return dst;
}

/// @brief memcpy definition in C++.
/// @param dst destination pointer.
/// @param  src source pointer.
/// @param len length of of src.
EXTERN_C VoidPtr memncpy(void* dst, const void* src, long long unsigned int len,
                         long long unsigned int dst_size) {
  if (!dst || !src || len > dst_size) {
    // Similar to memset, this is a critical failure.
    return nullptr;
  }
  unsigned char*       d = (unsigned char*) dst;
  const unsigned char* s = (const unsigned char*) src;
  for (size_t i = 0UL; i < len; ++i) {
    d[i] = s[i];
  }
  return dst;
}

/// @brief strlen definition in C++.
EXTERN_C size_t strnlen(const char* whatToCheck, size_t max_len) {
  size_t len = 0;
  while (len < max_len && whatToCheck[len] != '\0') {
    ++len;
  }
  return len;
}

/// @brief strcmp definition in C++.
EXTERN_C int strncmp(const char* whatToCheck, const char* whatToCheckRight, size_t max_len) {
  size_t i = 0;
  while (i < max_len && whatToCheck[i] == whatToCheckRight[i]) {
    if (whatToCheck[i] == '\0') return 0;
    ++i;
  }
  if (i == max_len) {
    return 0;
  }
  return (unsigned char) whatToCheck[i] - (unsigned char) whatToCheckRight[i];
}

/// @brief something specific to the Microsoft's ABI, When the stack grows too big.
EXTERN_C void ___chkstk_ms(void) {}

/// @note GCC expects them to be here.

/// @brief memset definition in C++.
/// @param dst destination pointer.
/// @param byte value to fill in.
/// @param len length of of src.
EXTERN_C VoidPtr memset(void* dst, int byte, long long unsigned int len) {
  for (size_t i = 0UL; i < len; ++i) {
    ((int*) dst)[i] = byte;
  }

  return dst;
}

/// @brief memcpy definition in C++.
/// @param dst destination pointer.
/// @param  src source pointer.
/// @param len length of of src.
EXTERN_C VoidPtr memcpy(void* dst, const void* src, long long unsigned int len) {
  for (size_t i = 0UL; i < len; ++i) {
    ((int*) dst)[i] = ((int*) src)[i];
  }

  return dst;
}

/// @brief strlen definition in C++.
EXTERN_C size_t strlen(const char* whatToCheck) {
  SizeT len = 0;

  while (whatToCheck[len] != 0) {
    ++len;
  }

  return len;
}

/// @brief strcmp definition in C++.
EXTERN_C int strcmp(const char* whatToCheck, const char* whatToCheckRight) {
  SizeT len = 0;

  while (whatToCheck[len] == whatToCheckRight[len]) {
    if (whatToCheck[len] == 0) return 0;

    ++len;
  }

  return len;
}

#endif
