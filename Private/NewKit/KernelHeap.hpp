/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

// last-rev 30/01/24
// file: KHeap.hpp
// description: heap allocation for the kernel.

#include <NewKit/Defines.hpp>
#include <NewKit/Pmm.hpp>

namespace HCore {
Int32 ke_delete_ke_heap(voidPtr allocatedPtr);
voidPtr ke_new_ke_heap(SizeT sz, const bool rw, const bool user);
}  // namespace HCore
