/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <BootKit/BootKit.h>
#include <CompilerKit/CompilerKit.h>
#include <modules/AHCI/AHCI.h>

#define kAHCISectorSz (512)

class BootDeviceSATA final {
 public:
  explicit BootDeviceSATA() noexcept;
  ~BootDeviceSATA() = default;

  NE_COPY_DEFAULT(BootDeviceSATA)

  struct SATATrait final : public Device::Trait  {
    Kernel::Boolean mErr{false};
    Kernel::Boolean mDetected{false};

    operator bool() { return !this->mErr; }
  };

  operator bool() { return this->Leak().mDetected; }

  SizeT GetDiskSize() { return drv_std_get_size(); }

  constexpr static auto kSectorSize = kAHCISectorSize;

  BootDeviceSATA& Read(Boot::CharacterTypeASCII* Buf, const Kernel::SizeT SecCount);
  BootDeviceSATA& Write(Boot::CharacterTypeASCII* Buf, const Kernel::SizeT SecCount);

  SATATrait& Leak();

 private:
  SATATrait mTrait;
};
