/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <BootKit/Device.h>
#include <modules/ATA/ATA.h>

using namespace Kernel;

class BootDeviceATA final : public Device {
 public:
  enum {
    kPrimary   = ATA_PRIMARY_IO,
    kSecondary = ATA_SECONDARY_IO,
  };

  explicit BootDeviceATA() noexcept;
  ~BootDeviceATA() = default;

  enum { kSectorSize = kATASectorSize };

  struct ATATrait final : public Device::Trait {
    UInt16  mBus{kPrimary};
    UInt8   mMaster{0};
    Boolean mErr{false};

    operator bool() { return !mErr; }
  };

 public:
  operator bool();

  SizeT GetSectorsCount() noexcept;
  SizeT GetDiskSize() noexcept;

  BootDeviceATA& Read(Char* Buf, SizeT SecCount) override;
  BootDeviceATA& Write(Char* Buf, SizeT SecCount) override;

  ATATrait& Leak() override;

 private:
  ATATrait mTrait;
};
