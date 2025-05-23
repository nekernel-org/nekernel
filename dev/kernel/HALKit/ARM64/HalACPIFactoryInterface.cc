/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/HeapMgr.h>
#include <NeKit/KString.h>
#include <modules/ACPI/ACPIFactoryInterface.h>
#include <modules/APM/APM.h>

namespace Kernel {
ACPIFactoryInterface::ACPIFactoryInterface(VoidPtr rsp_ptr) : fRsdp(rsp_ptr), fEntries(0) {}

BOOL ACPIFactoryInterface::Shutdown() {
  apm_send_io_command(kAPMPowerCommandShutdown, 0);
  return NO;
}

/// @brief Reboot machine in either ACPI or by triple faulting.
/// @return nothing it's a reboot.
Void ACPIFactoryInterface::Reboot() {
  apm_send_io_command(kAPMPowerCommandReboot, 0);
}
}  // namespace Kernel
