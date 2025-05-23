/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <NeKit/Defines.h>

#ifndef __TOOLCHAINKIT__

#define kAtExitMacDestructors (128)

struct atexit_func_entry_t {
  void (*destructor_func)();
  void* obj_ptr;
  void* dso_handle;
};

typedef unsigned uarch_t;

namespace cxxabiv1 {
typedef void* __guard;
}

#endif  // __GNUC__
