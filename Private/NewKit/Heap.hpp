/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/ArrayList.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/PageManager.hpp>
#include <NewKit/Pmm.hpp>
#include <NewKit/Ref.hpp>

// last-rev 5/11/23
// file: pool.hpp
// description: memory pool for user programs.

#define kPoolMaxSz 4096
#define kPoolMag   0x5500A1

namespace hCore
{
    enum
    {
        kPoolHypervisor = 0x2,
        kPoolShared = 0x4,
        kPoolUser = 0x6,
        kPoolRw = 0x8,
    };

    struct HeapHeader final
    {
        UInt32 Magic;
        Int32 Flags;
        Boolean Free;
        UIntPtr Pad;
    };

    VoidPtr pool_new_ptr(Int32 flags);
    Int32 pool_free_ptr(voidPtr pointer);
    Boolean pool_ptr_exists(UIntPtr thePool, UIntPtr thePtr, SizeT theLimit);
} // namespace hCore
