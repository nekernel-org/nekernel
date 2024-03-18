/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Builtins/ATA/Defines.hxx>

class BDeviceATA final {
 public:
  enum {
    kPrimary = ATA_PRIMARY_IO,
    kSecondary = ATA_SECONDARY_IO,
  };

  explicit BDeviceATA() noexcept;
  ~BDeviceATA() = default;

  HCORE_COPY_DEFAULT(BDeviceATA);

  struct ATATrait final {
    SizeT mBase{1024};
    UInt16 mBus{kPrimary};
    UInt8 mMaster{0};
    Boolean mErr{false};

    operator bool() { return !mErr; }
  };

  operator bool();

  BDeviceATA& Read(WideChar* Buf, const SizeT& SecCount);
  BDeviceATA& Write(WideChar* Buf, const SizeT& SecCount);

  ATATrait& Leak();

 private:
  ATATrait mTrait;
};

enum {
  kATADevicePATA,
  kATADeviceSATA,
  kATADevicePATA_PI,
  kATADeviceSATA_PI,
  kATADeviceCount,
};

