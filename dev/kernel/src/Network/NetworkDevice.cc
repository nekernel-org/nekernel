/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <NeKit/Utils.h>
#include <NetworkKit/NetworkDevice.h>

namespace Kernel {
/// \brief Getter for fNetworkName.
/// \return Network device name.
const Char* NetworkDevice::Name() const {
  return "/devices/net{}";
}

/// \brief Setter for fNetworkName.
Boolean NetworkDevice::Name(const Char* name) {
  if (name == nullptr) return NO;

  if (*name == 0) return NO;

  if (rt_string_len(name) > kNetworkNameLen) return NO;

  rt_copy_memory((VoidPtr) name, (VoidPtr) this->Name(), rt_string_len(this->Name()));

  return YES;
}
}  // namespace Kernel
