/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/String.hpp>
#include <StorageKit/PRDT.hpp>

namespace NewOS {
PRDT::PRDT(const UIntPtr& physAddr) : m_PrdtAddr(physAddr) {
  MUST_PASS(physAddr);
  kcout << "PRDT::PRDT() {}\r\n";
}

PRDT::~PRDT() {
  kcout << "PRDT::~PRDT() {}\r\n";
  m_PrdtAddr = 0;
}

const UInt& PRDT::Low() { return m_Low; }

const UShort& PRDT::High() { return m_High; }

const UIntPtr& PRDT::PhysicalAddress() { return m_PrdtAddr; }

PRDT& PRDT::operator=(const UIntPtr& prdtAddress) {
  m_PrdtAddr = prdtAddress;
  return *this;
}
}  // namespace NewOS
