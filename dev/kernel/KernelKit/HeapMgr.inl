/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifndef INC_KERNEL_HEAP_H
#include <KernelKit/HeapMgr.h>
#endif  // !INC_KERNEL_HEAP_H

namespace Kernel {
/// @brief Allocate C++ class.
/// @param cls The class to allocate.
/// @param args The args to pass.
template <typename T, typename... Args>
inline BOOL mm_new_class(_Input _Output T** cls, _Input Args&&... args) {
  if (*cls) {
    err_global_get() = Kernel::kErrorInvalidData;
    return NO;
  }

  *cls = new T(move(args)...);
  return *cls;
}

/// @brief Delete and nullify C++ class.
/// @param cls The class to delete.
template <typename T>
inline Void mm_delete_class(_Input _Output T** cls) {
  delete *cls;
  *cls = nullptr;
}
}  // namespace Kernel