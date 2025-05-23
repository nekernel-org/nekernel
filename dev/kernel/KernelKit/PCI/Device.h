/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <NeKit/Defines.h>

namespace Kernel::PCI {
enum class PciConfigKind : UShort {
  ConfigAddress = 0xCF8,
  ConfigData    = 0xCFC,
  CommandReg    = 0x0004,
  Invalid       = 0xFFFF,
};

/// @brief Device interface class
class Device final {
 public:
  Device() = default;

 public:
  Device(UShort bus, UShort device, UShort function, UInt32 bar);

  Device& operator=(const Device&) = default;
  Device(const Device&)            = default;

  ~Device();

 public:
  UInt Read(UInt bar, Size szData);
  void Write(UInt bar, UIntPtr data, Size szData);

 public:
  operator bool();

 public:
  template <typename T>
  UInt Read(UInt bar) {
    static_assert(sizeof(T) <= sizeof(UInt32), "64-bit PCI addressing is unsupported");
    return Read(bar, sizeof(T));
  }

  template <typename T>
  void Write(UInt bar, UIntPtr data) {
    static_assert(sizeof(T) <= sizeof(UInt32), "64-bit PCI addressing is unsupported");
    Write(bar, data, sizeof(T));
  }

 public:
  UShort  DeviceId();
  UShort  VendorId();
  UShort  InterfaceId();
  UChar   Class();
  UChar   Subclass();
  UChar   ProgIf();
  UChar   HeaderType();
  UIntPtr Bar(UInt32 bar_in);

 public:
  void EnableMmio();
  void BecomeBusMaster();  // for PCI-DMA, PC-DMA does not need that.

  UShort Vendor();

 private:
  UShort fBus;
  UShort fDevice;
  UShort fFunction;
  UInt32 fBar;
};
}  // namespace Kernel::PCI
