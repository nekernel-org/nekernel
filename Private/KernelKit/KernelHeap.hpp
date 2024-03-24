/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

// last-rev 30/01/24
// file: KernelHeap.hpp
// description: heap allocation for the kernel.

#include <NewKit/Defines.hpp>

namespace HCore {
Int32 ke_delete_ke_heap(voidPtr allocatedPtr);
Boolean ke_is_valid_heap(VoidPtr ptr);
voidPtr ke_new_ke_heap(SizeT sz, const bool rw, const bool user);
Boolean ke_protect_ke_heap(VoidPtr heapPtr);
}  // namespace HCore
