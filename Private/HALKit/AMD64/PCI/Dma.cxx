/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/PCI/Dma.hpp>

namespace HCore {
DMAWrapper::operator bool() { return m_Address; }

bool DMAWrapper::operator!() { return !m_Address; }

Boolean DMAWrapper::Check(UIntPtr offset) const {
  if (!m_Address) return false;
  if (offset == 0) return true;

  kcout << "[DMAWrapper::IsIn] Checking offset..\n";
  return reinterpret_cast<UIntPtr>(m_Address) >= offset;
}

bool DMAWrapper::Write(const UIntPtr &bit, const UIntPtr &offset) {
  if (!m_Address) return false;

  kcout << "[DMAWrapper::Write] Writing at address..\n";

  auto addr =
      (volatile UIntPtr *)(reinterpret_cast<UIntPtr>(m_Address) + offset);
  *addr = bit;

  return true;
}

UIntPtr DMAWrapper::Read(const UIntPtr &offset) {
  kcout << "[DMAWrapper::Read] checking m_Address..\n";
  if (!m_Address) return 0;

  kcout << "[DMAWrapper::Read] Reading m_Address..\n";
  return *(volatile UIntPtr *)(reinterpret_cast<UIntPtr>(m_Address) + offset);
  ;
}

UIntPtr DMAWrapper::operator[](const UIntPtr &offset) {
  return this->Read(offset);
}

OwnPtr<IOBuf<Char *>> DMAFactory::Construct(OwnPtr<DMAWrapper> &dma) {
  if (!dma) return {};

  OwnPtr<IOBuf<Char *>> dmaOwnPtr =
      make_ptr<IOBuf<Char *>, char *>(reinterpret_cast<char *>(dma->m_Address));

  if (!dmaOwnPtr) return {};

  kcout << "Returning the new OwnPtr<IOBuf<Char*>>!\r\n";
  return dmaOwnPtr;
}

DMAWrapper &DMAWrapper::operator=(voidPtr Ptr) {
  m_Address = Ptr;
  return *this;
}
}  // namespace HCore
