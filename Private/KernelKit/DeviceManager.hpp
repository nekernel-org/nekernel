/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/* -------------------------------------------

 Revision History:

     31/01/24: Add kDeviceCnt (amlel)

 ------------------------------------------- */

#pragma once

/* NewOS device interface manager. */
/* @file KernelKit/DeviceManager.hpp */
/* @brief Device abstraction and I/O buffer. */

#include <NewKit/ErrorOr.hpp>
#include <NewKit/Ref.hpp>

#define kDriveManagerCount 4U

// Last Rev
// Wed, Apr  3, 2024  9:09:41 AM

namespace NewOS {
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
  virtual DeviceInterface<T> &operator<<(T Data) {
    m_Out(Data);
    return *this;
  }

  virtual DeviceInterface<T> &operator>>(T Data) {
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

///
/// @brief Input Output Buffer
/// Used mainly to communicate between hardware.
///
template <typename T>
class IOBuf final {
 public:
  explicit IOBuf(T Dat) : m_Data(Dat) {
    // at least pass something valid when instancating this struct.
    MUST_PASS(Dat);
  }

  IOBuf &operator=(const IOBuf<T> &) = default;
  IOBuf(const IOBuf<T> &) = default;

  ~IOBuf() = default;

 public:
  template <typename R>
  R operator->() const {
    return m_Data;
  }

  template <typename R>
  R &operator[](Size index) const {
    return m_Data[index];
  }

 private:
  T m_Data;
};

///! @brief Device enum types.
enum {
  kDeviceTypeIDE,
  kDeviceTypeEthernet,
  kDeviceTypeWiFi,
  kDeviceTypeRS232,
  kDeviceTypeSCSI,
  kDeviceTypeSHCI,
  kDeviceTypeUSB,
  kDeviceTypeMedia,
  kDeviceTypeCount,
};
}  // namespace NewOS
