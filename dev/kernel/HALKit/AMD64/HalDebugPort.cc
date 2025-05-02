/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

//! @file DebuggerPort.cc
//! @brief UART debug via packets.

#include <ArchKit/ArchKit.h>
#include <KernelKit/DebugOutput.h>

// after that we have start of additional data.

namespace Kernel {
void rt_debug_listen(DebuggerPortHeader* theHook) noexcept {
  NE_UNUSED(theHook);
}
}  // namespace Kernel
