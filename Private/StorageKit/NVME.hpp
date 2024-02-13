/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <KernelKit/DeviceManager.hpp>
#include <NewKit/OwnPtr.hpp>

namespace HCore {
class NVMEPacket;

class NVMEDevice : public DeviceInterface<NVMEPacket> {
 public:
  explicit NVMEDevice(void (*Out)(NVMEPacket outpacket),
                      void (*In)(NVMEPacket inpacket), void (*Cleanup)(void))
      : DeviceInterface(Out, In), fCleanup(Cleanup) {}

  virtual ~NVMEDevice() {
    if (fCleanup) fCleanup();
  }

 public:
  NVMEDevice &operator=(const NVMEDevice &) = default;
  NVMEDevice(const NVMEDevice &) = default;

  virtual const char *Name() const;

 public:
  OwnPtr<NVMEPacket> operator()(UInt32 dmaLow, UInt32 dmaHigh, SizeT sz);

 private:
  void (*fCleanup)(void);
};

class NVMEPacket {
  UIntPtr DataPtr;
  SizeT DataSz;
  UInt64 DriveID;

  Boolean ErrorSet;
  Boolean BusySet;
  Boolean ReadySet;
};
}  // namespace HCore
