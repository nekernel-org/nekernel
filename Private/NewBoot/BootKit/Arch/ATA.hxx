/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Builtins/ATA/Defines.hxx>

using namespace NewOS;

class BootDeviceATA final {
 public:
  enum {
    kPrimary = ATA_PRIMARY_IO,
    kSecondary = ATA_SECONDARY_IO,
  };

  explicit BootDeviceATA() noexcept;
  ~BootDeviceATA() = default;

  HCORE_COPY_DEFAULT(BootDeviceATA);

  struct ATATrait final {
    SizeT mBase{1024};
    SizeT mSize{1024};
    UInt16 mBus{kPrimary};
    UInt8 mMaster{0};
    Boolean mErr{false};

    operator bool() { return !mErr; }
  };

  operator bool();

  BootDeviceATA& Read(Char* Buf, const SizeT& SecCount);
  BootDeviceATA& Write(Char* Buf, const SizeT& SecCount);

  ATATrait& Leak();

 private:
  ATATrait mTrait;
};
