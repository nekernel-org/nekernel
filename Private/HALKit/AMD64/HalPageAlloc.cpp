/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/KernelCheck.hpp>

STATIC NewOS::Boolean kAllocationInProgress = false;
namespace NewOS {
namespace HAL {
/// @brief Allocates a new page of memory.
/// @param sz the size of it.
/// @param rw read/write flag.
/// @param user user flag.
/// @return the page table of it.
STATIC auto hal_try_alloc_new_page(Boolean rw, Boolean user) -> VoidPtr {
  kAllocationInProgress = true;
  PTE* newAddress = (PTE*)kKernelVirtualStart;

  while (newAddress->Present) {
    newAddress = newAddress + sizeof(PTE);
  }

  newAddress->Present = true;
  newAddress->Rw = rw;
  newAddress->User = user;

  kAllocationInProgress = false;

  return reinterpret_cast<VoidPtr>(newAddress);
}

/// @brief Allocate a new page to be used by the OS.
/// @param rw
/// @param user
/// @return
auto hal_alloc_page(Boolean rw, Boolean user) -> VoidPtr {
  while (kAllocationInProgress) {
  }

  /// allocate new page.
  return hal_try_alloc_new_page(rw, user);
}
}  // namespace HAL
}  // namespace NewOS
