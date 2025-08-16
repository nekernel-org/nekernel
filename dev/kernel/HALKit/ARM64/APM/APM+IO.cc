/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/KPC.h>
#include <modules/APM/APM.h>

using namespace Kernel;

/// @brief Send APM command to its IO space.
/// @param base_dma the IO base port.
/// @param cmd the command.
/// @return status code.
EXTERN_C Int32 apm_send_io_command(UInt16 cmd) {
  switch (cmd) {
    case kAPMPowerCommandReboot: {
      asm volatile(
          "ldr x0, =0x84000004\n"
          "svc #0\n");

      return kErrorSuccess;
    }
    case kAPMPowerCommandShutdown: {
      asm volatile(
          "ldr x0, =0x84000008\n"
          "svc #0\n");

      return kErrorSuccess;
    }
    default:
      return kErrorInvalidData;
  }
}
