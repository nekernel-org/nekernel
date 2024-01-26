/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

// last-rev 5/03/23
// file: KHeap.hpp
// description: page allocation for kernel.

#include <NewKit/Defines.hpp>
#include <NewKit/Pmm.hpp>

namespace hCore
{
    Int32 kernel_delete_ptr(voidPtr ptr);
    voidPtr kernel_new_ptr(const SizeT& sz, const bool rw, const bool user);
} // namespace hCore
