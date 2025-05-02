/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/AHCI.h>

using namespace Kernel;

/// @brief Class constructor
/// @param Out Drive output
/// @param In  Drive input
/// @param Cleanup Drive cleanup.
AHCIDeviceInterface::AHCIDeviceInterface(void (*out)(IDeviceObject*       self,
                                                     MountpointInterface* outpacket),
                                         void (*in)(IDeviceObject*       self,
                                                    MountpointInterface* inpacket))
    : IDeviceObject(out, in) {}

/// @brief Class desctructor
AHCIDeviceInterface::~AHCIDeviceInterface() = default;

/// @brief Returns the name of the device interface.
/// @return it's name as a string.
const Char* AHCIDeviceInterface::Name() const {
  return "/devices/sda{}";
}

/// @brief Output operator.
/// @param mnt the disk mountpoint.
/// @return the class itself after operation.
AHCIDeviceInterface& AHCIDeviceInterface::operator<<(MountpointInterface* mnt) {
  if (!mnt) return *this;

  for (SizeT driveCount = 0; driveCount < kDriveMaxCount; ++driveCount) {
    auto interface = mnt->GetAddressOf(driveCount);

    if ((interface) &&
        rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) == 0) {
      continue;
    } else if ((interface) &&
               rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) != 0) {
      return *this;
    }
  }

  return (AHCIDeviceInterface&) IDeviceObject<MountpointInterface*>::operator<<(mnt);
}

/// @brief Input operator.
/// @param mnt the disk mountpoint.
/// @return the class itself after operation.
AHCIDeviceInterface& AHCIDeviceInterface::operator>>(MountpointInterface* mnt) {
  if (!mnt) return *this;

  for (SizeT driveCount = 0; driveCount < kDriveMaxCount; ++driveCount) {
    auto interface = mnt->GetAddressOf(driveCount);

    // really check if it's ATA.
    if ((interface) &&
        rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) == 0) {
      continue;
    } else if ((interface) &&
               rt_string_cmp((interface)->fProtocol(), "AHCI", rt_string_len("AHCI")) != 0) {
      return *this;
    }
  }

  return (AHCIDeviceInterface&) IDeviceObject<MountpointInterface*>::operator>>(mnt);
}

const UInt16& AHCIDeviceInterface::GetPortsImplemented() {
  return this->fPortsImplemented;
}

Void AHCIDeviceInterface::SetPortsImplemented(const UInt16& pi) {
  MUST_PASS(pi > 0);
  this->fPortsImplemented = pi;
}

const UInt32& AHCIDeviceInterface::GetIndex() {
  return this->fDriveIndex;
}

Void AHCIDeviceInterface::SetIndex(const UInt32& drv) {
  MUST_PASS(MountpointInterface::kDriveIndexInvalid != drv);
  this->fDriveIndex = drv;
}