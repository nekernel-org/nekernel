/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

namespace Kernel {
typedef Int32 APMPowerCmd;

enum {
  kAPMPowerCommandInvalid  = 0x00,
  kAPMPowerCommandStop     = 0x01,
  kAPMPowerCommandStart    = 0x02,
  kAPMPowerCommandSleep    = 0x04,
  kAPMPowerCommandWakeup   = 0x06,
  kAPMPowerCommandShutdown = 0x07,
  kAPMPowerCommandReboot   = 0x08,
};

/// @brief Send a APM command into it's own IO space.
/// @param base_dma the IO base port.
/// @param cmd the command.
/// @return status code.
EXTERN_C Int32 apm_send_io_command(UInt16 cmd);
}  // namespace Kernel
