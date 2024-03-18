/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <Builtins/AHCI/Defines.hxx>

class BDeviceSATA final {
 public:
  explicit BDeviceSATA() noexcept;
  ~BDeviceSATA() = default;

  HCORE_COPY_DEFAULT(BDeviceSATA);

  struct SATATrait final {
    HCore::SizeT   mBase{1024};
    HCore::Boolean mErr{false};
    HCore::Boolean mDetected{false};

    operator bool() { return !this->mErr; }
  };

  operator bool() { return this->Leak().mDetected; }

  BDeviceSATA& Read(HCore::WideChar* Buf, const HCore::SizeT& SecCount);
  BDeviceSATA& Write(HCore::WideChar* Buf, const HCore::SizeT& SecCount);

  SATATrait& Leak();

 private:
  SATATrait mTrait;
};

#define kAHCISectorSz 4096
