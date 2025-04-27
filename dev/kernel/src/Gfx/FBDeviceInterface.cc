/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <GfxKit/FB.h>

using namespace Kernel;

/// @brief Class constructor
/// @param Out Drive output
/// @param In  Drive input
/// @param Cleanup Drive cleanup.
FBDeviceInterface::FBDeviceInterface(void (*out)(IDeviceObject* self, FBDevicePacket* outpacket),
                                     void (*in)(IDeviceObject* self, FBDevicePacket* inpacket))
    : IDeviceObject(out, in) {}

/// @brief Class desctructor
FBDeviceInterface::~FBDeviceInterface() = default;

/// @brief Output operator.
/// @param mnt the disk mountpoint.
/// @return the class itself after operation.
FBDeviceInterface& FBDeviceInterface::operator<<(FBDevicePacket* pckt) {
  if (!pckt) return *this;

  if (pckt->fHeight == 0 || pckt->fWidth == 0) return *this;

  this->fOut(this, pckt);

  return *this;
}

/// @brief Input operator.
/// @param mnt the disk mountpoint.
/// @return the class itself after operation.
FBDeviceInterface& FBDeviceInterface::operator>>(FBDevicePacket* pckt) {
  if (!pckt) return *this;

  this->fIn(this, pckt);

  return *this;
}

/// @brief Returns the name of the device interface.
/// @return it's name as a string.
const Char* FBDeviceInterface::Name() const {
  return "/devices/fb{}";
}