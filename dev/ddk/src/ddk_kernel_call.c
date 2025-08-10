/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  Purpose: DDK Kernel call.

------------------------------------------- */

#include <DDKKit/ddk.h>
#include <stdarg.h>

/// @brief this is an internal call, do not use it.
DDK_EXTERN ATTRIBUTE(naked) void* ke_call_dispatch(const char* name, int32_t cnt, void* data,
                                                   size_t sz);

/// @brief Interupt Kernel and call it's RPC.
/// @param KernelRpcName RPC name
/// @param cnt number of elements in **data** pointer.
/// @param data data pointer.
/// @param sz The size of the whole data pointer.
/// @retval void* Kernel call was successful.
/// @retval nil Kernel call failed, call KernelLastError(void)
DDK_EXTERN void* ke_call(const char* name, int32_t cnt, void* data, size_t sz) {
  if (!name || *name == 0 || cnt == 0) return nil;

  return ke_call_dispatch(name, cnt, data, sz);
}

/// @brief Add system call.
/// @param slot system call slot
/// @param slotFn, syscall slot.
DDK_EXTERN void ke_set_syscall(const int slot, void (*slotFn)(void* a0)) {
  ke_call("ke_set_syscall", slot, slotFn, 1);
}

/// @brief Get a Kernel object.
/// @param slot property id (always 0)
/// @param name the object's name.
/// @return Object manifest.
DDK_EXTERN struct DDK_OBJECT_MANIFEST* ke_get_obj(const int slot, const char* name) {
  struct DDK_OBJECT_MANIFEST* manifest =
      (struct DDK_OBJECT_MANIFEST*) ke_call("cfkit_get_kobj", slot, (void*) name, 1);

  if (!manifest) return nil;

  return manifest;
}

/// @brief Set a Kernel object.
/// @param slot property id (always 0)
/// @param name the object's name.
/// @param ddk_pr pointer to a object's DDK_OBJECT_MANIFEST.
/// @return property's object.
DDK_EXTERN void* ke_set_obj(const int slot, const struct DDK_OBJECT_MANIFEST* ddk_pr) {
  return ke_call("cfkit_set_kobj", slot, (void*) ddk_pr, 1);
}
