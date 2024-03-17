/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <Drivers/AHCI/Defines.hxx>

class BDeviceSATA final {
 public:
  explicit BDeviceSATA() noexcept;
  ~BDeviceSATA() = default;

  HCORE_COPY_DEFAULT(BDeviceSATA);

  struct AHCITraits final {
    HCore::SizeT   mBase{1024};
    HCore::Boolean mErr{false};
    HCore::Boolean mDetected{false};

    operator bool() { return !this->mErr; }
  };

  operator bool() { return this->Leak().mDetected; }

  BDeviceSATA& Read(HCore::WideChar* Buf, const HCore::SizeT& SecCount);
  BDeviceSATA& Write(HCore::WideChar* Buf, const HCore::SizeT& SecCount);

  AHCITraits& Leak();

 private:
  AHCITraits mTraits;
};

#define kAHCISectorSz 512
