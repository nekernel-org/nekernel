/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

//
// Created by Amlal on 1/27/24.
//

#ifndef __THREAD_API__
#define __THREAD_API__

#include <NewKit/Defines.hpp>

namespace HCore {
/// @brief Thread Information Block for Local Storage.
/// Located in GS on AMD64, Virtual Address 0x10000 (64x0, 32x0, ARM64)
struct ThreadInformationBlock final {
  const Char Name[255];        // Module Name
  const UIntPtr StartAddress;  // Start Address
  const UIntPtr StartHeap;     // Allocation Heap
  const UIntPtr StartStack;    // Stack Pointer.
  const Int32 Arch;            // Architecture and/or platform.
};

enum {
  kPC_IA64,
  kPC_AMD64 = kPC_IA64,
  kPC_ARM,
  kMACS_64x0,
  kMACS_32x0,
};
}  // namespace HCore

#endif  // __THREAD_API__
