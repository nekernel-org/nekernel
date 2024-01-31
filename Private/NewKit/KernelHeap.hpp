/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

// last-rev 30/01/24
// file: KHeap.hpp
// description: heap allocation for the kernel.

#include <NewKit/Defines.hpp>
#include <NewKit/Pmm.hpp>

namespace HCore {
Void ke_init_ke_heap() noexcept;
Int32 ke_delete_ke_heap(voidPtr allocatedPtr);
voidPtr ke_new_ke_heap(const SizeT &sz, const bool rw, const bool user);
}  // namespace HCore
