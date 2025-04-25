/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <user/SystemCalls.h>
#include <cstdarg>

IMPORT_C VoidPtr sci_syscall_arg_1(SizeT id);
IMPORT_C VoidPtr sci_syscall_arg_2(SizeT id, VoidPtr arg1);
IMPORT_C VoidPtr sci_syscall_arg_3(SizeT id, VoidPtr arg1, VoidPtr arg3);
IMPORT_C VoidPtr sci_syscall_arg_4(SizeT id, VoidPtr arg1, VoidPtr arg3, VoidPtr arg4);
