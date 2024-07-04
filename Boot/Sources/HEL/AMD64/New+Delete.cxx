/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

#ifdef __STANDALONE__
#include <cstddef> /* Since we're using GCC for this EFI program. */

/// @brief Allocates a new object.
/// @param sz the size.
/// @return
void* operator new(size_t sz)
{
	void* buf = nullptr;
	BS->AllocatePool(EfiMemoryType::EfiLoaderData, sz, &buf);

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

/// @brief Deletes the object (array specific).
/// @param buf the object.
/// @param size it's size.
void operator delete(void* buf, size_t size)
{
	BS->FreePool(buf);
}

#endif // Inactive
