/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <NewKit/OwnPtr.hpp>

namespace NewOS {
class ATAPacket;

class ATADeviceInterface : public DeviceInterface<ATAPacket> {
 public:
  explicit ATADeviceInterface(void (*Out)(ATAPacket outpacket),
                              void (*In)(ATAPacket inpacket),
                              void (*Cleanup)(void));

  virtual ~ATADeviceInterface();

 public:
  ATADeviceInterface &operator=(const ATADeviceInterface &) = default;
  ATADeviceInterface(const ATADeviceInterface &) = default;

  const char *Name() const override;

 private:
  void (*fOut)(ATAPacket);
  void (*fIn)(ATAPacket);
  void (*fCleanup)(void);
};

class ATAPacket final {
  UIntPtr DataPtr;
  SizeT DataSz;
  UInt8 PortId;
  UInt8 PortRdy;
  Lba BeginLba;
  Lba SectorCnt;
};
}  // namespace NewOS