/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

#ifndef PTE_MAX
#define PTE_MAX (0x200)
#endif  //! PTE_MAX

#ifndef PTE_ALIGN
#define PTE_ALIGN (0x1000)
#endif  //! PTE_ALIGN

extern "C" void flush_tlb(HCore::UIntPtr VirtualAddr);
extern "C" void write_cr3(HCore::UIntPtr pde);
extern "C" void write_cr0(HCore::UIntPtr bit);

extern "C" HCore::UIntPtr read_cr0();
extern "C" HCore::UIntPtr read_cr2();
extern "C" HCore::UIntPtr read_cr3();

namespace HCore::HAL {
struct PageTable64 {
  bool Present : 1;
  bool Rw : 1;
  bool User : 1;
  bool Wt : 1;
  bool Cache : 1;
  bool Accessed : 1;
  HCore::Int32 Reserved : 6;
  HCore::UIntPtr PhysicalAddress : 36;
  HCore::Int32 Reserved1 : 15;
  bool ExecDisable : 1;
};

struct PageDirectory64 final {
  PageTable64 ALIGN(PTE_ALIGN) Pte[PTE_MAX];
};

PageTable64* hal_alloc_page(SizeT sz, Boolean rw, Boolean user);
UIntPtr& hal_page_base() noexcept;
void hal_page_base(const UIntPtr& newPagePtr) noexcept;
UIntPtr hal_create_page(Boolean rw, Boolean user);
}  // namespace HCore::HAL
