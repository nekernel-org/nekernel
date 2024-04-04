/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Builtins/ATA/Defines.hxx>
#include <BootKit/Device.hxx>

using namespace NewOS;

class BootDeviceATA final : public Device {
 public:
  enum {
    kPrimary = ATA_PRIMARY_IO,
    kSecondary = ATA_SECONDARY_IO,
  };

  explicit BootDeviceATA() noexcept;
  ~BootDeviceATA() = default;

  HCORE_COPY_DEFAULT(BootDeviceATA);

  struct ATATrait final : public Device::Trait {
    UInt16 mBus{kPrimary};
    UInt8 mMaster{0};
    Boolean mErr{false};

    operator bool() { return !mErr; }
  };

 public:
  operator bool();

  BootDeviceATA& Read(Char* Buf, const SizeT& SecCount) override;
  BootDeviceATA& Write(Char* Buf, const SizeT& SecCount) override;

  ATATrait& Leak() override;

 private:
  ATATrait mTrait;
};