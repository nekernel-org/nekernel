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
namespace Detail {
struct VirtualMemoryHeader {
  Boolean Present : 1;
  Boolean ReadWrite : 1;
  Boolean User : 1;
};

struct VirtualMemoryHeaderTraits {
  /// @brief Get next header.
  /// @param current 
  /// @return 
  VirtualMemoryHeader* Next(VirtualMemoryHeader* current) {
    return current + sizeof(PTE);
  }

  /// @brief Get previous header.
  /// @param current 
  /// @return 
  VirtualMemoryHeader* Prev(VirtualMemoryHeader* current) {
    return current - sizeof(PTE);
  }
};
}

/// @brief Allocates a new page of memory.
/// @param sz the size of it.
/// @param rw read/write flag.
/// @param user user flag.
/// @return the page table of it.
STATIC auto hal_try_alloc_new_page(Boolean rw, Boolean user) -> VoidPtr {
  if (kAllocationInProgress) return nullptr;

  kAllocationInProgress = true;

  ///! fetch from the start.
  Detail::VirtualMemoryHeader* vmHeader = reinterpret_cast<Detail::VirtualMemoryHeader*>(kKernelVirtualStart);
  Detail::VirtualMemoryHeaderTraits traits;

  while (vmHeader->Present) {
    vmHeader = traits.Next(vmHeader);
  }

  vmHeader->Present = true;
  vmHeader->ReadWrite = rw;
  vmHeader->User = user;

  kAllocationInProgress = false;

  return reinterpret_cast<VoidPtr>(vmHeader);
}

/// @brief Allocate a new page to be used by the OS.
/// @param rw read/write bit.
/// @param user user bit.
/// @return
auto hal_alloc_page(Boolean rw, Boolean user) -> VoidPtr {
  /// Wait for a ongoing allocation to complete.
  while (kAllocationInProgress) {
    ;
  }

  /// allocate new page.
  return hal_try_alloc_new_page(rw, user);
}
}  // namespace HAL
}  // namespace NewOS
