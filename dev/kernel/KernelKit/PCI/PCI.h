/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define kPCIConfigAddressPort (0xCF8)
#define kPCIConfigDataPort (0xCFC)

#define kPCIDeviceCount (32)
#define kPCIFuncCount (8)
#define kPCIBusCount (256U)

namespace Kernel::PCI {
// model
struct DeviceHeader {
  UInt16 VendorId;
  UInt16 DeviceId;
  UInt8  Command;
  UInt8  Status;
  UInt8  RevisionId;
  UInt8  ProgIf;
  UInt8  SubClass;
  UInt8  Class;
  UInt8  CacheLineSz;
  UInt8  LatencyTimer;
  UInt8  HeaderType;
  UInt8  Bist;
  UInt8  Bus;
  UInt8  Device;
  UInt8  Function;
};

namespace Detail {
  class BAR {
   public:
    UIntPtr BAR;
    SizeT   Size;
  };
}  // namespace Detail

class BAR {
 public:
  Detail::BAR BAR1;
  Detail::BAR BAR2;
  Detail::BAR BAR3;
  Detail::BAR BAR4;
  Detail::BAR BAR5;
};
}  // namespace Kernel::PCI
