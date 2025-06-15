/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <modules/AHCI/AHCI.h>
#include <modules/ATA/ATA.h>

using namespace Kernel;

/// @brief Device type.
class Device {
 public:
  explicit Device() = default;
  virtual ~Device() = default;

  NE_MOVE_DEFAULT(Device)

  struct Trait {
    SizeT mBase{0};
    SizeT mSize{0};
  };

  virtual Trait& Leak() = 0;

  virtual Device& Read(Char* Buf, SizeT SecCount)  = 0;
  virtual Device& Write(Char* Buf, SizeT SecCount) = 0;
};

typedef Device BootDevice;
typedef Device NetworkDevice;
typedef Device DiskDevice;
