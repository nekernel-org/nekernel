/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <BootKit/BootKit.h>
#include <CompilerKit/CompilerKit.h>
#include <modules/AHCI/AHCI.h>

class BootDeviceSATA final {
 public:
  explicit BootDeviceSATA() noexcept;
  ~BootDeviceSATA() = default;

  NE_COPY_DEFAULT(BootDeviceSATA)

  struct SATATrait final {
    Kernel::SizeT   mBase{1024};
    Kernel::Boolean mErr{false};
    Kernel::Boolean mDetected{false};
    Kernel::SizeT   mSize{0};

    operator bool() { return !this->mErr; }
  };

  operator bool() { return this->Leak().mDetected; }

  SizeT GetDiskSize() { return drv_std_get_size(); }

  constexpr static auto kSectorSize = kAHCISectorSize;

  BootDeviceSATA& Read(Boot::CharacterTypeUTF8* Buf, const Kernel::SizeT SecCount);
  BootDeviceSATA& Write(Boot::CharacterTypeUTF8* Buf, const Kernel::SizeT SecCount);

  SATATrait& Leak();

 private:
  SATATrait mTrait;
};

#define kAHCISectorSz 4096
