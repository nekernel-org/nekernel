/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <NeKit/Defines.h>

#ifndef __NECTI__

#define kAtExitMacDestructors (128)

struct atexit_func_entry_t {
  void (*destructor_func)();
  void* obj_ptr;
  void* dso_handle;
};

typedef Kernel::UInt32 uarch_t;

namespace cxxabiv1 {
typedef Kernel::SizeT* __guard;
}

#endif  // !__NECTI__
