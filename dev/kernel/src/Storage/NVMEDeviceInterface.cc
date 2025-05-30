/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/NVME.h>

namespace Kernel {
NVMEDeviceInterface::NVMEDeviceInterface(
    void (*out)(DeviceInterface*, MountpointInterface* outpacket),
    void (*in)(DeviceInterface*, MountpointInterface* inpacket), void (*cleanup)(void))
    : DeviceInterface(out, in), fCleanup(cleanup) {}

NVMEDeviceInterface::~NVMEDeviceInterface() {
  if (fCleanup) fCleanup();
}

const Char* NVMEDeviceInterface::Name() const {
  return ("/devices/nvme{}");
}
}  // namespace Kernel
