/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <NewKit/OwnPtr.hpp>

namespace NewOS {
class NVMEPacket;

class NVMEDeviceInterface : public DeviceInterface<NVMEPacket> {
 public:
  explicit NVMEDeviceInterface(void (*Out)(NVMEPacket outpacket),
                      void (*In)(NVMEPacket inpacket), void (*Cleanup)(void))
      : DeviceInterface(Out, In), fCleanup(Cleanup) {}

  virtual ~NVMEDeviceInterface() {
    if (fCleanup) fCleanup();
  }

 public:
  NVMEDeviceInterface &operator=(const NVMEDeviceInterface &) = default;
  NVMEDeviceInterface(const NVMEDeviceInterface &) = default;

  const char *Name() const override;

 public:
  OwnPtr<NVMEPacket> operator()(UInt32 dmaLow, UInt32 dmaHigh, SizeT sz);

 private:
  void (*fCleanup)(void);
};

class NVMEPacket final {
  UIntPtr DataPtr;
  SizeT   DataSz;
  UInt8   Namespace;
  Lba     Begin;
  Lba     End;
};
}  // namespace NewOS
