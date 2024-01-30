/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

// last-rev 30/01/24
// file: KHeap.hpp
// description: heap allocation for the kernel.

#include <NewKit/Defines.hpp>
#include <NewKit/Pmm.hpp>

namespace hCore {
Int32 kernel_delete_ptr(voidPtr allocatedPtr);
voidPtr kernel_new_ptr(const SizeT &sz, const bool rw, const bool user);
}  // namespace hCore
