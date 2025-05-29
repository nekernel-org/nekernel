/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/***
  Dtor and ctors.
*/

#ifndef __INC_NETWORK_DEVICE_H__
#include <NetworkKit/NetworkDevice.h>
#endif  // __INC_NETWORK_DEVICE_H__

namespace Kernel {
inline NetworkDevice::NetworkDevice(void (*out)(IDeviceObject<NetworkDeviceCommand>*,
                                                NetworkDeviceCommand),
                                    void (*in)(IDeviceObject<NetworkDeviceCommand>*,
                                               NetworkDeviceCommand),
                                    void (*on_cleanup)(void))
    : IDeviceObject<NetworkDeviceCommand>(out, in), fCleanup(on_cleanup) {
  kout << "NetworkDevice initialized.\r";

  MUST_PASS(out && in && on_cleanup);
}

inline NetworkDevice::~NetworkDevice() {
  kout << "NetworkDevice cleanup.\r";

  if (fCleanup) fCleanup();
}
}  // namespace Kernel
