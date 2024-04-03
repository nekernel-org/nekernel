/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <KernelKit/DriveManager.hxx>
#include <NewKit/OwnPtr.hpp>

namespace NewOS {
class NVMEDeviceInterface : public DeviceInterface<MountpointInterface*> {
 public:
  explicit NVMEDeviceInterface(void (*Out)(MountpointInterface* outpacket),
                      void (*In)(MountpointInterface* inpacket), void (*Cleanup)(void))
      : DeviceInterface(Out, In), fCleanup(Cleanup) {}

  virtual ~NVMEDeviceInterface() {
    if (fCleanup) fCleanup();
  }

 public:
  NVMEDeviceInterface &operator=(const NVMEDeviceInterface &) = default;
  NVMEDeviceInterface(const NVMEDeviceInterface &) = default;

  const char *Name() const override;

 public:
  OwnPtr<MountpointInterface*> operator()(UInt32 dmaLow, UInt32 dmaHigh, SizeT sz);

 private:
  void (*fCleanup)(void);
};
}  // namespace NewOS
