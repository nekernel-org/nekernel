/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>
#include <BootKit/BootKit.h>

#ifdef __BOOTLDR_STANDALONE__
EXTERN EfiBootServices* BS;

/// @brief Allocates a new object.
/// @param sz the size.
/// @return
void* operator new(size_t sz)
{
	void* buf = nullptr;

	while (BS->AllocatePool(EfiMemoryType::EfiLoaderData, sz, &buf) == kBufferTooSmall)
		BS->FreePool(buf);

	return buf;
}

/// @brief Allocates a new object.
/// @param sz the size.
/// @return
void* operator new[](size_t sz)
{
	void* buf = nullptr;
	BS->AllocatePool(EfiMemoryType::EfiLoaderData, sz, &buf);

	return buf;
}

/// @brief Deletes the object.
/// @param buf the object.
void operator delete(void* buf)
{
	BS->FreePool(buf);
}

/// @brief Deletes the object.
/// @param buf the object.
void operator delete[](void* buf)
{
	BS->FreePool(buf);
}

/// @brief Deletes the object (array specific).
/// @param buf the object.
/// @param size it's size.
void operator delete(void* buf, size_t size)
{
	BS->FreePool(buf);
}

#endif // __BOOTLDR_STANDALONE__
