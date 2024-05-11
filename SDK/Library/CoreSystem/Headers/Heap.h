/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

#define kAllocationTypes 2

enum RtAllocationKind
{
	kStandardAllocation = 0xC,
	kArrayAllocation	= 0xD,
};

/// @brief Allocates a new pointer from process pool.
/// @param sz the size
/// @param flags the allocation flags.
/// @return
CA_EXTERN_C PtrVoidType RtHeapAllocate(QWordType sz,
									  DWordType flags);

/// @brief Check if the pointer exists.
/// @param ptr the pointer to free.
/// @return
CA_EXTERN_C BooleanType RtHeapPtrExists(PtrVoidType ptr);

/// @brief Gets the size of the process' pointer.
/// @param ptr the pointer to free.
/// @return
CA_EXTERN_C QWordType RtHeapGetSize(PtrVoidType ptr);

/// @brief Frees the process pointer.
/// @param ptr the pointer to free.
/// @return
CA_EXTERN_C VoidType RtHeapFree(PtrVoidType ptr);
