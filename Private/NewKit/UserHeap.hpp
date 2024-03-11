/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/ArrayList.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/PageManager.hpp>
#include <NewKit/Pmm.hpp>
#include <NewKit/Ref.hpp>

// last-rev 5/11/23
// file: UserHeap.hpp
// description: memory heap for user programs.

#define kPoolMaxSz (4096)
#define kPoolMag 0x5500A1

namespace HCore {
typedef enum {
  kPoolHypervisor = 0x2,
  kPoolShared = 0x4,
  kPoolUser = 0x6,
  kPoolRw = 0x8,
} kPoolFlags;

/// @brief Allocate a process heap, no zero out is done here.
/// @param flags 
/// @return The process's heap.
VoidPtr rt_new_heap(Int32 flags);


Int32 rt_free_heap(voidPtr pointer);
}  // namespace HCore
