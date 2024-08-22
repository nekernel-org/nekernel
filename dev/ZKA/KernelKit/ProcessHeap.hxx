/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Array.hxx>
#include <NewKit/ArrayList.hxx>
#include <NewKit/ErrorOr.hxx>
#include <NewKit/PageManager.hxx>
#include <NewKit/Ref.hxx>
#include <NewKit/Pmm.hxx>

/// @version 5/11/23
/// @file ProcessHeap.hxx
/// @brief Process heap allocator.

#define kProcessHeapMag   (0xFAF0FEF0)

namespace Kernel
{
	typedef enum
	{
		/// @brief Shared heap.
		kProcessHeapShared = 0x4,
		/// @brief User and private heap.
		kProcessHeapUser = 0x6,
		/// @brief Read and Write heap.
		kProcessHeapRw = 0x8,
	} UserHeapFlags;

	/// @brief Allocate a process heap, no zero out is done here.
	/// @param flags the heap's flags.
	/// @param initial_len_in_gib the initial heap's length in GB.
	/// @return The process's heap.
	VoidPtr sched_new_heap(Int32 flags, SizeT initial_len_in_gib);

	/// @brief Frees the process heap.
	/// @param pointer The process heap pointer.
	/// @return status code of the freeing.
	Int32 sched_free_heap(voidPtr pointer);
} // namespace Kernel
