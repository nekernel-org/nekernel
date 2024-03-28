/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/ArrayList.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/PageManager.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Pmm.hpp>

/// @version 5/11/23
/// @file UserHeap.hpp
/// @brief memory heap for user programs.

#define kUserHeapMaxSz (4096)
#define kUserHeapMag (0x5500A1)

namespace NewOS {
typedef enum {
  kUserHeapHypervisor = 0x2,
  kUserHeapShared = 0x4,
  kUserHeapUser = 0x6,
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
}  // namespace NewOS
