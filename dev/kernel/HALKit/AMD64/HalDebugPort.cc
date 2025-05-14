/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

//! @file DebuggerPort.cc
//! @brief UART debug via packets.

#include <ArchKit/ArchKit.h>
#include <KernelKit/DebugOutput.h>
#include <NetworkKit/NetworkDevice.h>

// after that we have start of additional data.

namespace Kernel {
Void rt_debug_listen(KernelDebugHeader* the_hdr) noexcept {
  NE_UNUSED(the_hdr);
}
}  // namespace Kernel
