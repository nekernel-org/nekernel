/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <libSystem/SystemKit/Syscall.h>
#include <libSystem/SystemKit/System.h>

/// @file SystemAPI.cc
/// @brief System wide API for NeKernel.

IMPORT_C VoidPtr MmCopyMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len) {
  if (!len || !dest || !src) {
    return nullptr;
  }

  for (SizeT i = 0; i < len; i++) {
    ((Char*) dest)[i] = ((Char*) src)[i];
  }

  return dest;
}

IMPORT_C SInt64 MmStrLen(const Char* in) {
  if (!in) return 0;

  SizeT len{0};

  do {
    ++len;
  } while (in[len] != '\0');

  return len;
}

IMPORT_C VoidPtr MmFillMemory(_Input VoidPtr dest, _Input SizeT len, _Input UInt8 value) {
  if (!len || !dest) {
    return nullptr;
  }

  for (SizeT i = 0; i < len; i++) {
    ((Char*) dest)[i] = value;
  }

  return dest;
}

IMPORT_C Ref IoOpenFile(_Input const Char* path, _Input const Char* drv_letter) {
  return (Ref) libsys_syscall_arg_3(SYSCALL_HASH("IoOpenFile"),
                                    reinterpret_cast<VoidPtr>(const_cast<Char*>(path)),
                                    reinterpret_cast<VoidPtr>(const_cast<Char*>(drv_letter)));
}

IMPORT_C Void IoCloseFile(_Input Ref desc) {
  libsys_syscall_arg_2(SYSCALL_HASH("IoCloseFile"), desc);
}

IMPORT_C UInt64 IoSeekFile(_Input Ref desc, _Input UInt64 off) {
  auto ret = (volatile UInt64*) libsys_syscall_arg_3(
      SYSCALL_HASH("IoSeekFile"), reinterpret_cast<VoidPtr>(desc), reinterpret_cast<VoidPtr>(&off));

  MUST_PASS((*ret) != ~0UL);
  return *ret;
}

IMPORT_C UInt64 IoTellFile(_Input Ref desc) {
  auto ret = (volatile UInt64*) libsys_syscall_arg_2(SYSCALL_HASH("IoTellFile"),
                                                     reinterpret_cast<VoidPtr>(desc));
  return *ret;
}

IMPORT_C SInt32 PrintOut(_Input IORef desc, const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);

  auto ret = (volatile UInt64*) libsys_syscall_arg_4(
      SYSCALL_HASH("PrintOut"), reinterpret_cast<VoidPtr>(desc),
      reinterpret_cast<VoidPtr>(const_cast<Char*>(fmt)), args);

  va_end(args);

  return *ret;
}

IMPORT_C Void _rtl_assert(Bool expr, const Char* origin) {
  if (!expr) {
    PrintOut(nullptr, "Assertion failed: %s\r", origin);
    PrintOut(nullptr, "Origin: %s\r", origin);

    libsys_syscall_arg_1(SYSCALL_HASH("_rtl_debug_break"));
  }
}
