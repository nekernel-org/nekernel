// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-foss/ne-kernel

#include <SystemKit/Err.h>
#include <SystemKit/Syscall.h>
#include <SystemKit/Verify.h>

/// @brief This file handles the SCI's assertion API.

using namespace System;

IMPORT_C Void _rtl_assert(Bool expr, const Char* origin) {
#ifndef _NDEBUG
  if (!expr) {
    PrintOut(nullptr, "Assertion failed: %s\r", origin ? origin : "");
    nesys_syscall_arg_1(SYSCALL_HASH("_rtl_debug_break"));
  }
#endif
}