/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/ArrayList.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/PageManager.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Pmm.hpp>

/// @version 5/11/23
/// @file ProcessHeap.hxx
/// @brief Heap for user processes.

#define kUserHeapMaxSz (4096)
#define kUserHeapMag   (0xFAF0FEF0)

namespace Kernel
{
	typedef enum
	{
		/// @brief Shared heap.
		kUserHeapShared = 0x4,
		/// @brief User and private heap.
		kUserHeapUser = 0x6,
		/// @brief Read and Write heap.
		kUserHeapRw = 0x8,
	} kUserHeapFlags;

	/// @brief Allocate a process heap, no zero out is done here.
	/// @param flags
	/// @return The process's heap.
	VoidPtr rt_new_heap(Int32 flags);

	/// @brief Frees the process heap.
	/// @param pointer The process heap pointer.
	/// @return
	Int32 rt_free_heap(voidPtr pointer);
} // namespace Kernel
