/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <Drivers/ATA/Defines.hxx>

Boolean IsATADetected(Void);

class BDeviceATA final {
 public:
  enum {
    kPrimary = ATA_PRIMARY_IO,
    kSecondary = ATA_SECONDARY_IO,
  };

  explicit BDeviceATA() noexcept;
  ~BDeviceATA() = default;

  HCORE_COPY_DEFAULT(BDeviceATA);

  struct ATATraits final {
    SizeT mBase{1024};
    UInt16 mBus{kPrimary};
    UInt8 mMaster{0};
    Boolean mErr{false};

    operator bool() { return !mErr; }
  };

  operator bool() { return IsATADetected(); }

  BDeviceATA& Read(WideChar* Buf, const SizeT& SecCount);
  BDeviceATA& Write(WideChar* Buf, const SizeT& SecCount);

  ATATraits& Leak();

 private:
  ATATraits mTraits;
};

enum {
  kATADevicePATA,
  kATADeviceSATA,
  kATADevicePATA_PI,
  kATADeviceSATA_PI,
  kATADeviceCount,
};

#define kATASectorSz 512
