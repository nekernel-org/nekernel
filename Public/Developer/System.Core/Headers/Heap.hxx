/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Defines.hxx>

#define kAllocationTypes 2

enum RtAllocationKind {
  kStandardAllocation = 0xC,
  kArrayAllocation = 0xD,
};

/// @brief Allocates a new heap from process pool.
/// @param refObj 
/// @param sz 
/// @param flags 
/// @return 
CA_EXTERN_C PtrVoidType RtAllocateProcessHeap(ObjectRef refObj, QWordType sz,
                                              DWordType flags);

/// @brief Check if pointer exists.
/// @param refObj 
/// @param ptr 
/// @return 
CA_EXTERN_C BooleanType RtProcessHeapExists(ObjectRef refObj, PtrVoidType ptr);
CA_EXTERN_C QWordType RtProcessHeapSize(ObjectRef refObj, PtrVoidType ptr);
CA_EXTERN_C VoidType RtFreeProcessHeap(ObjectRef refObj, PtrVoidType ptr);
