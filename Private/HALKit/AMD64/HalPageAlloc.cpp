/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/KernelCheck.hpp>

// this files handles paging.

STATIC HCore::SizeT kPageCnt = 0UL;
STATIC HCore::Boolean kAllocationInProgress = false;

#define kKernelPagingPadding kPTEAlign

namespace HCore {
namespace HAL {
/// @brief Allocates a new page of memory.
/// @param sz the size of it.
/// @param rw read/write flag.
/// @param user user flag.
/// @return the page table of it.
STATIC auto hal_try_alloc_new_page(SizeT sz, Boolean rw, Boolean user)
    -> PageTable64 * {
  kAllocationInProgress = true;
  MUST_PASS(sz > 0);

  PageTable64 *pte = reinterpret_cast<PageTable64 *>(kKernelVirtualStart);

  pte->Rw = rw;
  pte->User = user;
  pte->Present = true;
  pte->PhysicalAddress = (UIntPtr)kKernelPhysicalStart;

  kKernelVirtualStart = (VoidPtr)((UIntPtr)kKernelVirtualStart + kKernelPagingPadding);
  kKernelPhysicalStart = (VoidPtr)((UIntPtr)kKernelPhysicalStart + kKernelPagingPadding);

  ++kPageCnt;

  kAllocationInProgress = false;
  return pte;
}

auto hal_alloc_page(SizeT sz, Boolean rw, Boolean user) -> PageTable64 * {
  if (sz == 0)
    ++sz;

  /// allocate new page.
  return hal_try_alloc_new_page(sz, rw, user);
}

auto hal_create_page(Boolean rw, Boolean user) -> UIntPtr {
  while (kAllocationInProgress) {}

  PageTable64 *new_pte = hal_alloc_page(sizeof(PageTable64), rw, user);
  MUST_PASS(new_pte);

  return reinterpret_cast<UIntPtr>(new_pte);
}
}  // namespace HAL
}  // namespace HCore
