// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#include <KernelKit/HeapMgr.h>
#include <KernelKit/KPC.h>
#include <NeKit/KernelPanic.h>

namespace Ne::Kernel {
STATIC Bool kRaiseOnBugCheck = false;

/// @brief Does a system wide bug check.
/// @param void no params are needed.
/// @return if error-free: false, otherwise true.
Boolean err_bug_check_raise(Void) {
  Char* ptr = new Char[512];

  if (ptr == nullptr) goto err_bug_check_fail;

  if (!mm_is_valid_ptr(ptr)) goto err_bug_check_fail;

  if (ptr) delete[] ptr;

  return Yes;

err_bug_check_fail:
  if (ptr) delete[] ptr;

  ptr = nullptr;

  if (kRaiseOnBugCheck) {
    ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
  }

  return No;
}
}  // namespace Ne::Kernel
