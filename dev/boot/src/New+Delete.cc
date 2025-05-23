/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/BootKit.h>
#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>
#include <FirmwareKit/EFI/API.h>

#ifdef __BOOTZ_STANDALONE__

/// @brief Allocates a new object.
/// @param sz the size.
/// @return
void* operator new(size_t sz) {
  void* buf = nullptr;

  while (BS->AllocatePool(EfiMemoryType::EfiLoaderData, sz, &buf) != kEfiOk)
    ;

  return buf;
}

/// @brief Allocates a new object.
/// @param sz the size.
/// @return
void* operator new[](size_t sz) {
  void* buf = nullptr;
  BS->AllocatePool(EfiMemoryType::EfiLoaderData, sz, &buf);

  return buf;
}

/// @brief Deletes the object.
/// @param buf the object.
void operator delete(void* buf) {
  if (!buf) return;

  BS->FreePool(buf);
}

/// @brief Deletes the object.
/// @param buf the object.
void operator delete[](void* buf) {
  if (!buf) return;

  BS->FreePool(buf);
}

/// @brief Deletes the object (array specific).
/// @param buf the object.
/// @param size it's size.
void operator delete(void* buf, size_t size) {
  if (!buf) return;

  NE_UNUSED(size);

  BS->FreePool(buf);
}

/// @brief Deletes the object (array specific).
/// @param buf the object.
/// @param size it's size.
void operator delete[](void* buf, size_t size) {
  if (!buf) return;

  NE_UNUSED(size);

  BS->FreePool(buf);
}

#endif  // __BOOTZ_STANDALONE__
