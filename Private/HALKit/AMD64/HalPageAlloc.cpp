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
    -> VoidPtr {
  MUST_PASS(sz > 0);

  kAllocationInProgress = true;
  PDE* cr3 = (PDE*)hal_read_cr3();

  kcout << "HCoreKrnl.exe: CR3: " << hex_number((UIntPtr)cr3) << endl;

  for (size_t i = 0; i < kPTESize; ++i)
  {
    if (cr3->Pte[i].Present) continue;
    kcout << "HCoreKrnl.exe: Page index: " << hex_number(i) << endl;

    cr3->Pte[i].Rw = rw;
    cr3->Pte[i].User = user;
    cr3->Pte[i].Present = true;

    ++kPageCnt;

    kAllocationInProgress = false;
    kcout << "HCoreKrnl.exe: Allocation done for: " << hex_number(i) << endl;
    return (VoidPtr)cr3->Pte[i].PhysicalAddress;
  }
  
  kAllocationInProgress = false;
  return nullptr;
}

auto hal_alloc_page(SizeT sz, Boolean rw, Boolean user) -> VoidPtr {
  while (kAllocationInProgress) {}

  if (sz == 0)
    ++sz;

  /// allocate new page.
  return hal_try_alloc_new_page(sz, rw, user);
}

auto hal_create_page(Boolean rw, Boolean user) -> UIntPtr {
  return reinterpret_cast<UIntPtr>(hal_alloc_page(sizeof(PageTable64), rw, user));
}
}  // namespace HAL
}  // namespace HCore
