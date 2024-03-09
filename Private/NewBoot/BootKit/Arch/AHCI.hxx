/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/CompilerKit.hpp>
#include <Drivers/AHCI/Defines.hxx>

class BDeviceAHCI final {
 public:
  explicit BDeviceAHCI() noexcept;
  ~BDeviceAHCI() = default;

  HCORE_COPY_DEFAULT(BDeviceAHCI);

  struct AHCITraits final {
    HCore::SizeT mBase{1024};
    HCore::Boolean mErr{false};
    HCore::Boolean mDetected{false};

    operator bool() { return !mErr; }
  };

  operator bool() { return Leak().mDetected; }

  BDeviceAHCI& Read(HCore::WideChar* Buf, const HCore::SizeT& SecCount);
  BDeviceAHCI& Write(HCore::WideChar* Buf, const HCore::SizeT& SecCount);

  AHCITraits& Leak();

 private:
  AHCITraits mTraits;
};

#define kAHCISectorSz 512
