/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

#define cAllocationKindCount (2U)

enum CsAllocationKind
{
	kStandardAllocation = 0xC,
	kArrayAllocation	= 0xD,
};

/// @brief Allocates a new pointer from process pool.
/// @param sz the size
/// @param flags the allocation flags.
/// @return
CS_EXTERN_C PtrVoidType CSAllocateHeap(QWordType sz,
									   DWordType flags);

/// @brief Check if the pointer exists.
/// @param ptr the pointer to free.
/// @return
CS_EXTERN_C BooleanType CSIsHeapValid(PtrVoidType ptr);

/// @brief Gets the size of the process' pointer.
/// @param ptr the pointer to free.
/// @return
CS_EXTERN_C QWordType CSGetHeapSize(PtrVoidType ptr);

/// @brief Frees the process pointer.
/// @param ptr the pointer to free.
/// @return
CS_EXTERN_C VoidType CSFreeHeap(PtrVoidType ptr);
