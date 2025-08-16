/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>
#include <cstdarg>

IMPORT_C VoidPtr libsys_syscall_arg_1(SizeT id);
IMPORT_C VoidPtr libsys_syscall_arg_2(SizeT id, VoidPtr arg1);
IMPORT_C VoidPtr libsys_syscall_arg_3(SizeT id, VoidPtr arg1, VoidPtr arg3);
IMPORT_C VoidPtr libsys_syscall_arg_4(SizeT id, VoidPtr arg1, VoidPtr arg3, VoidPtr arg4);

inline UInt64 libsys_hash_64(const Char* path) {
  if (!path || *path == 0) return 0;

  const UInt64 FNV_OFFSET_BASIS = 0xcbf29ce484222325ULL;
  const UInt64 FNV_PRIME        = 0x100000001b3ULL;

  UInt64 hash = FNV_OFFSET_BASIS;

  while (*path) {
    hash ^= (Char) (*path++);
    hash *= FNV_PRIME;
  }

  return hash;
}

#ifndef SYSCALL_HASH
#define SYSCALL_HASH(str) libsys_hash_64(str)
#endif  // !SYSCALL_HASH