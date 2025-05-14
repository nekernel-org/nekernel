/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/***
  Dtor and ctors.
*/

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
  MUST_PASS(fCleanup);

  kout << "NetworkDevice cleanup.\r";

  if (fCleanup) fCleanup();
}
}  // namespace Kernel
