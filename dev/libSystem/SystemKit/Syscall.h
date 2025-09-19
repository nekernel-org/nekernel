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

IMPORT_C UInt64 libsys_hash_64(const Char* path);

#ifndef SYSCALL_HASH
#define SYSCALL_HASH(str) libsys_hash_64(str)
#endif  // !SYSCALL_HASH
