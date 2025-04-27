/* -------------------------------------------

  Copyright Amlal El Mahrouss.

  FILE: ddk.h
  PURPOSE: DDK Driver model base header.

------------------------------------------- */

#pragma once

#include <DDKKit/macros.h>

struct DDK_STATUS_STRUCT;
struct DDK_OBJECT_MANIFEST;

/// \brief Object handle manifest.
struct DDK_OBJECT_MANIFEST DDK_FINAL {
  char*   p_name;
  int32_t p_kind;
  void*   p_object;
};

/// \brief DDK status ping structure.
struct DDK_STATUS_STRUCT DDK_FINAL {
  int32_t                     s_action_id;
  int32_t                     s_issuer_id;
  int32_t                     s_group_id;
  struct DDK_OBJECT_MANIFEST* s_object;
};

/// @brief Call Kernel procedure.
/// @param name the procedure name.
/// @param cnt number of elements in **dat**
/// @param dat data argument pointer.
/// @param sz sz of whole data argument pointer.
/// @return result of call
DDK_EXTERN void* ke_call(const char* name, int32_t cnt, void* dat, size_t sz);

/// @brief add a system call.
/// @param slot system call slot id.
/// @param slotFn, syscall slot.
DDK_EXTERN void ke_add_syscall(const int32_t slot, void (*slotFn)(void* a0));

/// @brief Allocates an heap ptr.
/// @param sz size of the allocated struct/type.
/// @return the pointer allocated or **nil**.
DDK_EXTERN void* kalloc(size_t sz);

/// @brief Frees an heap ptr.
/// @param pointer kernel pointer to free.
DDK_EXTERN void kfree(void* the_ptr);

/// @brief Gets a Kernel object.
/// @param slot object id (can be 0)
/// @param name the property's name.
/// @return DDK_OBJECT_MANIFEST.
DDK_EXTERN struct DDK_OBJECT_MANIFEST* ke_get_obj(const int slot, const char* name);

/// @brief Set a Kernel object.
/// @param slot object id (can be 0)
/// @param name the property's name.
/// @param ddk_pr pointer to a object's DDK_OBJECT_MANIFEST.
/// @return returned object.
DDK_EXTERN void* ke_set_obj(const int32_t slot, const struct DDK_OBJECT_MANIFEST* ddk_pr);

/// @brief The highest API version of the DDK.
DDK_EXTERN int32_t kApiVersionHighest;

/// @brief The lowest API version of the DDK.
DDK_EXTERN int32_t kApiVersionLowest;

/// @brief API version in BCD.
DDK_EXTERN int32_t kApiVersion;
