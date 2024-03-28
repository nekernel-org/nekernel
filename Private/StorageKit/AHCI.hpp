/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <NewKit/OwnPtr.hpp>

namespace NewOS {
class AHCIPacket;

class AHCIDeviceInterface : public DeviceInterface<AHCIPacket> {
 public:
   explicit AHCIDeviceInterface(void (*Out)(AHCIPacket outpacket),
                      void (*In)(AHCIPacket inpacket), void (*Cleanup)(void));

  virtual ~AHCIDeviceInterface();

 public:
  AHCIDeviceInterface &operator=(const AHCIDeviceInterface &) = default;
  AHCIDeviceInterface(const AHCIDeviceInterface &) = default;

  const char *Name() const override;

  private:
  void (*fCleanup)(void);
};

class AHCIPacket final {
  UIntPtr DataPtr;
  SizeT   DataSz;
  UInt8   PortId;
  UInt8   PortRdy;
  Lba     Begin;
  Lba     End;
};
} // namespace NewOS