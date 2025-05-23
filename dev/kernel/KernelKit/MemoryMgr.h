/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef INC_KERNEL_HEAP_H
#define INC_KERNEL_HEAP_H

/// @date 30/01/24
/// @file: MemoryMgr.h
/// @brief: Memory allocation support for the NeKernel.

#include <KernelKit/KPC.h>
#include <NeKit/KernelPanic.h>
#include <hint/CompilerHint.h>

namespace Kernel {
/// @brief Declare pointer as free.
/// @param heap_ptr the pointer.
/// @return a status code regarding the deallocation.
Int32 mm_delete_ptr(VoidPtr heap_ptr);

/// @brief Check if pointer is a valid Kernel pointer.
/// @param heap_ptr the pointer
/// @return if it exists it returns true.
Boolean mm_is_valid_ptr(VoidPtr heap_ptr);

/// @brief Allocate chunk of memory.
/// @param sz Size of pointer
/// @param wr Read Write bit.
/// @param user User enable bit.
/// @return The newly allocated pointer, or nullptr.
VoidPtr mm_new_ptr(SizeT sz, Bool wr, Bool user, SizeT pad_amount = 0);

/// @brief Protect the heap with a CRC value.
/// @param heap_ptr pointer.
/// @return if it valid: point has crc now., otherwise fail.
Boolean mm_protect_ptr(VoidPtr heap_ptr);

/// @brief Makes a Kernel page.
/// @param heap_ptr the page pointer.
/// @return status code
Int32 mm_make_page(VoidPtr heap_ptr);

/// @brief Overwrites and set the flags of a heap header.
/// @param heap_ptr the pointer to update.
/// @param flags the flags to set.
Int32 mm_make_ptr_flags(VoidPtr heap_ptr, UInt64 flags);

/// @brief Gets the flags of a heap header.
/// @param heap_ptr the pointer to get.
UInt64 mm_get_ptr_flags(VoidPtr heap_ptr);

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

#endif  // !INC_KERNEL_HEAP_H
