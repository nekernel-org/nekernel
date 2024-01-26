/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

#ifndef PTE_MAX
#define PTE_MAX (512)
#endif //! PTE_MAX

#ifndef PTE_ALIGN
#define PTE_ALIGN (4096)
#endif //! PTE_ALIGN

extern "C" void flush_tlb(hCore::UIntPtr VirtualAddr);
extern "C" void write_cr3(hCore::UIntPtr pde);
extern "C" void write_cr0(hCore::UIntPtr bit);

extern "C" hCore::UIntPtr read_cr0();
extern "C" hCore::UIntPtr read_cr2();
extern "C" hCore::UIntPtr read_cr3();

namespace hCore::HAL
{
    struct PageTable64
    {
        bool Present: 1;
        bool Rw: 1;
        bool User: 1;
        bool Wt: 1;
        bool Cache: 1;
        bool Accessed: 1;
        hCore::Int32 Reserved: 6;
        hCore::UIntPtr PhysicalAddress: 36;
        hCore::Int32 Reserved1: 15;
        bool ExecDisable: 1;

    };

    PageTable64 *hal_alloc_page(SizeT sz, Boolean rw, Boolean user);

    UIntPtr hal_create_page(Boolean rw, Boolean user);
} // namespace hCore::HAL
