/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/* -------------------------------------------

 Revision History:

     31/01/24: Add kDeviceCnt (amlel)

 ------------------------------------------- */

#pragma once

/* HCore */
/* File: KernelKit/Device.hpp */
/* Device abstraction and I/O buffer. */

#include <NewKit/ErrorOr.hpp>
#include <NewKit/Ref.hpp>

namespace HCore {
template <typename T>
class DeviceInterface;

template <typename T>
class DeviceInterface {
 public:
  explicit DeviceInterface(void (*Out)(T), void (*In)(T))
      : m_Out(Out), m_In(In) {}

  virtual ~DeviceInterface() = default;

 public:
  DeviceInterface &operator=(const DeviceInterface<T> &) = default;
  DeviceInterface(const DeviceInterface<T> &) = default;

 public:
  DeviceInterface<T> &operator<<(T Data) {
    m_Out(Data);
    return *this;
  }

  DeviceInterface<T> &operator>>(T Data) {
    m_In(Data);
    return *this;
  }

  virtual const char *Name() const { return "DeviceInterface"; }

  operator bool() { return m_Out && m_In; }
  bool operator!() { return !m_Out && !m_In; }

 private:
  void (*m_Out)(T Data);
  void (*m_In)(T Data);
};

template <typename T>
class IOBuf final {
 public:
  explicit IOBuf(T Dat) : m_Data(Dat) {}

  IOBuf &operator=(const IOBuf<T> &) = default;
  IOBuf(const IOBuf<T> &) = default;

  ~IOBuf() = default;

 public:
  T operator->() const { return m_Data; }
  T &operator[](Size index) const { return m_Data[index]; }

 private:
  T m_Data;
};

///! @brief Device types enum.
enum {
  kDeviceTypeIDE,
  kDeviceTypeEthernet,
  kDeviceTypeWiFi,
  kDeviceTypeRS232,
  kDeviceTypeSCSI,
  kDeviceTypeSHCI,
  kDeviceTypeUSB,
  kDeviceTypeCount,
};
}  // namespace HCore
