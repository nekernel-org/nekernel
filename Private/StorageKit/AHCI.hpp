/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <NewKit/OwnPtr.hpp>

namespace HCore {
class AHCIPacket;

class AHCIDevice : public DeviceInterface<AHCIPacket> {
 public:
};

class AHCIPacket final {
  UIntPtr DataPtr;
  SizeT   DataSz;
  UInt8   PortId;
  UInt8   PortRdy;
  Lba     Begin;
  Lba     End;
};
} // namespace HCore