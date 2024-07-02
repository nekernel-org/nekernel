/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

// last-rev 30/01/24
// file: KernelHeap.hpp
// description: heap allocation for the kernel.

#include <NewKit/Defines.hpp>

namespace Kernel
{
	/// @brief Declare pointer as free.
	/// @param heapPtr the pointer.
	/// @return
	Int32 ke_delete_ke_heap(voidPtr allocatedPtr);

	/// @brief Check if pointer is a valid kernel pointer.
	/// @param heapPtr the pointer
	/// @return if it exists.
	Boolean ke_is_valid_heap(VoidPtr ptr);

	/// @brief allocate chunk of memory.
	/// @param sz size of pointer
	/// @param rw read write (true to enable it)
	/// @param user is it accesible by user processes?
	/// @return the pointer
	voidPtr ke_new_ke_heap(SizeT sz, const bool rw, const bool user);

	/// @brief Protect the heap with a CRC value.
	/// @param heapPtr HIB pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean ke_protect_ke_heap(VoidPtr heapPtr);

	/// @brief Makes a kernel heap page.
	/// @param heapPtr
	/// @return
	Int32 ke_make_ke_page(VoidPtr heapPtr);
} // namespace Kernel
