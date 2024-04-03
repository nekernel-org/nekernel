/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/Utils.hpp>

namespace NewOS {
/// @brief ATA device interface type.
class ATADeviceInterface : public DeviceInterface<MountpointInterface*> {
 public:
  explicit ATADeviceInterface(void (*Out)(MountpointInterface* outpacket),
                              void (*In)(MountpointInterface* inpacket),
                              void (*Cleanup)(void));

  virtual ~ATADeviceInterface();

public:
  ATADeviceInterface &operator<<(MountpointInterface* Data) override;
  ATADeviceInterface &operator>>(MountpointInterface* Data) override;

 public:
  ATADeviceInterface &operator=(const ATADeviceInterface &) = default;
  ATADeviceInterface(const ATADeviceInterface &) = default;

  const char *Name() const override;

 private:
  void (*fCleanup)(void);
};
}  // namespace NewOS