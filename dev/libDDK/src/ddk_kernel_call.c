/* -------------------------------------------

  DDK
  Copyright Amlal El Mahrouss.

  Author: Amlal El Mahrouss
  Purpose: DDK kernel dispatch system.

------------------------------------------- */

#include <DriverKit/ddk.h>
#include <stdarg.h>

/// @brief this is an internal call, do not use it.
DDK_EXTERN ATTRIBUTE(naked) ptr_t __ke_call_dispatch(const int32_t name, int32_t cnt, void* data,
                                                     size_t sz);
/// @brief This function hashes the path into a FNV symbol.
/// @param path the path to hash.
/// @retval 0 symbol wasn't hashed.
/// @retval > 0 hashed symbol.
static uint64_t ddk_fnv_64(const char* path) {
  if (path == nil || *path == 0) return 0;

  const uint64_t kFnvOffsetBase = 0xcbf29ce484222325ULL;
  const uint64_t kFnvPrime64    = 0x100000001b3ULL;

  uint64_t hash = kFnvOffsetBase;

  while (*path) {
    hash ^= (char) (*path++);
    hash *= kFnvPrime64;
  }

  return hash;
}

/// @brief Interrupt Kernel and call it's RPC.
/// @param name RPC name
/// @param cnt number of elements in **data** pointer.
/// @param data data pointer.
/// @param sz The size of the whole data pointer.
/// @retval void* Kernel call was successful.
/// @retval nil Kernel call failed, call KernelLastError(void)
DDK_EXTERN void* ke_call_dispatch(const char* name, int32_t cnt, void* data, size_t sz) {
  if (name == nil || *name == 0 || data == nil || cnt == 0) return nil;
  return __ke_call_dispatch(ddk_fnv_64(name), cnt, data, sz);
}

/// @brief Add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DDK_EXTERN void ke_set_syscall(const int slot, void (*slotFn)(void* a0)) {
  ke_call_dispatch("ke_set_syscall", slot, slotFn, 1);
}

/// @brief Get a Kernel object.
/// @param slot property id (always 0)
/// @param name the object's name.
/// @return Object manifest.
DDK_EXTERN struct DDK_OBJECT_MANIFEST* ke_get_obj(const int slot, const char* name) {
  struct DDK_OBJECT_MANIFEST* manifest =
      (struct DDK_OBJECT_MANIFEST*) ke_call_dispatch("cfkit_get_kobj", slot, (void*) name, 1);

  if (!manifest) return nil;

  return manifest;
}

/// @brief Set a Kernel object.
/// @param slot property id (always 0)
/// @param name the object's name.
/// @param ddk_pr pointer to a object's DDK_OBJECT_MANIFEST.
/// @return property's object.
DDK_EXTERN void* ke_set_obj(const int slot, const struct DDK_OBJECT_MANIFEST* ddk_pr) {
  return ke_call_dispatch("cfkit_set_kobj", slot, (void*) ddk_pr, 1);
}
