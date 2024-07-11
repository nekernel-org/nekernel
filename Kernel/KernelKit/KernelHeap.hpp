/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

// last-rev 30/01/24
// file: KernelHeap.hpp
// description: heap allocation for the kernel.

#include <NewKit/Defines.hpp>

namespace Kernel
{
	/// @brief Declare pointer as free.
	/// @param allocatedPtr the pointer.
	/// @return
	Int32 ke_delete_ke_heap(voidPtr allocatedPtr);

	/// @brief Declare a new size for allocatedPtr.
	/// @param allocatedPtr the pointer.
	/// @return
	voidPtr ke_realloc_ke_heap(voidPtr allocatedPtr, SizeT newSz);

	/// @brief Check if pointer is a valid kernel pointer.
	/// @param allocatedPtr the pointer
	/// @return if it exists.
	Boolean ke_is_valid_heap(VoidPtr allocatedPtr);

	/// @brief allocate chunk of memory.
	/// @param sz size of pointer
	/// @param rw read write (true to enable it)
	/// @param user is it accesible by user processes?
	/// @return the pointer
	voidPtr ke_new_ke_heap(const SizeT sz, const Bool rw, const Bool user);

	/// @brief Protect the heap with a CRC value.
	/// @param allocatedPtr pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean ke_protect_ke_heap(VoidPtr allocatedPtr);

	/// @brief Makes a kernel heap page.
	/// @param allocatedPtr the page pointer.
	/// @return
	Int32 ke_make_ke_page(VoidPtr allocatedPtr);
} // namespace Kernel
