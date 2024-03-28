/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/PCI/Dma.hpp>
#include <KernelKit/PCI/Iterator.hpp>

#define kPrdtTransferSize (sizeof(NewOS::UShort))

namespace NewOS {
class PRDT final {
 public:
  explicit PRDT() = delete;
  explicit PRDT(const UIntPtr &physAddr);
  ~PRDT();

  PRDT &operator=(const PRDT &) = default;
  PRDT(const PRDT &) = default;

 public:
  const UInt &Low();
  const UShort &High();
  const UIntPtr &PhysicalAddress();

 public:
  PRDT &operator=(const UIntPtr &prdtAddress);

 public:
  operator bool() { return m_PrdtAddr != 0; }

 private:
  union {
    UInt m_Low;
    UShort m_High;
  };

  UIntPtr m_PrdtAddr;
};

using PhysicalAddress = PRDT;  // here
}  // namespace NewOS
