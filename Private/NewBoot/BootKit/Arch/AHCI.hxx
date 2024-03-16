/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <Drivers/AHCI/Defines.hxx>

class BDeviceAHCI final {
 public:
  explicit BDeviceAHCI() noexcept;
  ~BDeviceAHCI() = default;

  HCORE_COPY_DEFAULT(BDeviceAHCI);

  struct AHCITraits final {
    HCore::SizeT   fBase{1024};
    HCore::Boolean fError{false};
    HCore::Boolean fDetected{false};

    operator bool() { return !this->fError; }
  };

  operator bool() { return this->Leak().fDetected; }

  BDeviceAHCI& Read(HCore::WideChar* Buf, const HCore::SizeT& SecCount);
  BDeviceAHCI& Write(HCore::WideChar* Buf, const HCore::SizeT& SecCount);

  AHCITraits& Leak();

 private:
  AHCITraits mTraits;
};

#define kAHCISectorSz 512
