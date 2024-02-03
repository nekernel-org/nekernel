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
    SizeT mBase{1024};
    Boolean mErr{false};
    Boolean mDetected{false};

    operator bool() { return !mErr; }
  };

  operator bool() { return Leak().mDetected; }

  BDeviceAHCI& Read(WideChar* Buf, const SizeT& SecCount);
  BDeviceAHCI& Write(WideChar* Buf, const SizeT& SecCount);

  AHCITraits& Leak();

 private:
  AHCITraits mTraits;
};

enum {
  kATADevicePATA,
  kATADeviceSATA,
  kATADevicePATA_PI,
  kATADeviceSATA_PI,
  kATADeviceCount,
};

#define kATASectorSz 512
