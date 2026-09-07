// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#include <KernelKit/FileMgr.h>
#include <SwapKit/DiskSwap.h>

namespace Ne::Kernel {

/// @brief SMM Internal swap disk check function.
EXTERN_C Bool smmi_check_swap_dsk(const struct SwapDiskHdr* hdr) {
  if (hdr && hdr->fBlobSz > 0) return YES;
  return NO;
}

}  // namespace Ne::Kernel
