/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: HalPagingMgr.cc
  Purpose: Platform Paging Manager.

------------------------------------------- */

#include <HALKit/ARM64/Paging.h>
#include <HALKit/ARM64/Processor.h>

namespace Kernel::HAL {
typedef UInt32 PageTableIndex;

/// @brief Maps or allocates a page from virtual_address.
/// @param virtual_address a valid virtual address.
/// @param phys_addr point to physical address.
/// @param flags the flags to put on the page.
/// @return Status code of page manipulation process.
EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags) {
  if (!virtual_address || !flags) return kErrorInvalidData;

  NE_UNUSED(physical_address);

  return kErrorSuccess;
}
}  // namespace Kernel::HAL
