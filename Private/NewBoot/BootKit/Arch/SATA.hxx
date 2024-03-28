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
    NewOS::SizeT   mBase{1024};
    NewOS::Boolean mErr{false};
    NewOS::Boolean mDetected{false};

    operator bool() { return !this->mErr; }
  };

  operator bool() { return this->Leak().mDetected; }

  BDeviceSATA& Read(NewOS::WideChar* Buf, const NewOS::SizeT& SecCount);
  BDeviceSATA& Write(NewOS::WideChar* Buf, const NewOS::SizeT& SecCount);

  SATATrait& Leak();

 private:
  SATATrait mTrait;
};

#define kAHCISectorSz 4096
