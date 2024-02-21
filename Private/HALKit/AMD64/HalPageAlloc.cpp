/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/RuntimeCheck.hpp>

// this files handles paging.

static HCore::SizeT kPageCnt = 0UL;

#define kPagePad 512

namespace HCore {
namespace HAL {
static auto hal_try_alloc_new_page(SizeT sz, Boolean rw, Boolean user)
    -> PageTable64 * {
  PageTable64 *pte = reinterpret_cast<PageTable64 *>(
      (UIntPtr)kKernelVirtualStart + kPageCnt + kPagePad);

  pte->Rw = rw;
  pte->User = user;
  pte->Present = true;

  kKernelVirtualStart = (VoidPtr)((UIntPtr)kKernelVirtualStart + kPageCnt + sz);
  return pte;
}

auto hal_alloc_page(SizeT sz, Boolean rw, Boolean user) -> PageTable64 * {
  for (SizeT i = 0; i < kPageCnt; ++i) {
    PageTable64 *pte = reinterpret_cast<PageTable64 *>(
        (UIntPtr)kKernelVirtualStart + kPageCnt);

    if (!pte->Present) {
      pte->User = user;
      pte->Rw = rw;
      pte->Present = true;

      return pte;
    }
  }

  return hal_try_alloc_new_page(sz, rw, user);
}

auto hal_create_page(Boolean rw, Boolean user) -> UIntPtr {
  PageTable64 *new_pte = hal_alloc_page(sizeof(PageTable64), rw, user);
  MUST_PASS(new_pte);

  return reinterpret_cast<UIntPtr>(new_pte);
}
}  // namespace HAL
}  // namespace HCore
