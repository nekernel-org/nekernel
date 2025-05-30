/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/* -------------------------------------------

 Revision History:

  31/01/24: Add kDeviceCnt (amlel)
  15/11/24: Add NE_DEVICE macro, to inherit from device object.

 ------------------------------------------- */

#pragma once

/* @note Device Mgr. */
/* @file KernelKit/DeviceMgr.h */
/* @brief Device abstraction and I/O buffer. */

#include <NeKit/ErrorOr.h>
#include <NeKit/Ref.h>

#define kDeviceMgrRootDirPath "/devices/"

#define NE_DEVICE : public ::Kernel::DeviceInterface

// Last Rev: Wed, May  27, 2025  6:22 PM

namespace Kernel {
template <typename T>
class DeviceInterface;

template <typename T>
class IOBuf;

/***********************************************************************************/
/// @brief Device contract interface, represents an HW device.
/***********************************************************************************/
template <typename T>
class DeviceInterface {
 public:
  DeviceInterface() = default;
  
  explicit DeviceInterface(void (*Out)(DeviceInterface<T>*, T), void (*In)(DeviceInterface<T>*, T))
      : fOut(Out), fIn(In) {}

  virtual ~DeviceInterface() = default;

 public:
  DeviceInterface& operator=(const DeviceInterface<T>&) = default;
  DeviceInterface(const DeviceInterface<T>&)            = default;

 public:
  virtual DeviceInterface<T>& operator<<(T Data) {
    fOut(this, Data);
    return *this;
  }

  virtual DeviceInterface<T>& operator>>(T Data) {
    fIn(this, Data);
    return *this;
  }

  virtual const char* Name() const { return "/devices/null"; }

  operator bool() { return fOut && fIn; }

  Bool operator!() { return !fOut || !fIn; }

 protected:
  Void (*fOut)(DeviceInterface<T>*, T Data) = {nullptr};
  Void (*fIn)(DeviceInterface<T>*, T Data)  = {nullptr};
};

///
/// @brief Input Output abstract class.
/// Used mainly to communicate between OS to hardware.
///
template <typename T>
class IOBuf final {
 public:
  explicit IOBuf(T dma_addr) : fData(dma_addr) {
    // At least pass something valid when instancating this struct.
    MUST_PASS(fData);
  }

  IOBuf& operator=(const IOBuf<T>&) = default;
  IOBuf(const IOBuf<T>&)            = default;

  ~IOBuf() = default;

 public:
  template <typename R>
  R operator->() const {
    return fData;
  }

  template <typename R>
  R& operator[](Size index) const {
    return fData[index];
  }

 private:
  T fData;
};

///! @brief Device enum types.
enum {
  kDeviceTypeInvalid = 0,
  kDeviceTypeIDE     = 100,
  kDeviceTypeEthernet,
  kDeviceTypeWiFi,
  kDeviceTypeFW,
  kDeviceTypeBT,
  kDeviceTypeRS232,
  kDeviceTypeSCSI,
  kDeviceTypeAHCI,
  kDeviceTypeMBCI,
  kDeviceTypeATA,
  kDeviceTypeUSB,
  kDeviceTypeAPM,  // Adv. Pwr. Mgmt.
  kDeviceTypePCI,
  kDeviceTypeVGA,
  kDeviceTypeGPU,
  kDeviceTypeCount,
};
}  // namespace Kernel
