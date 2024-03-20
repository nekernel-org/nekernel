/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/PageManager.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#endif  // ifdef __x86_64__

//! null deref will throw (Page Zero detected, aborting app!)
#define kProtectedRegionEnd (512)

namespace HCore {
PTEWrapper::PTEWrapper(Boolean Rw, Boolean User, Boolean ExecDisable,
                       UIntPtr VirtAddr)
    : m_Rw(Rw),
      m_User(User),
      m_ExecDisable(ExecDisable),
      m_VirtAddr(VirtAddr),
      m_Cache(false),
      m_Shareable(false),
      m_Wt(false),
      m_Present(true),
      m_Accessed(false) {
  // special case for the null region.
  if (VirtAddr <= kProtectedRegionEnd) {
    m_Wt = false;
    m_Rw = false;
    m_Cache = false;
    m_Shareable = false;
    m_ExecDisable = true;
  }
}

PTEWrapper::~PTEWrapper() {
  PDE* cr3 = (PDE*)hal_read_cr3();

  PTE* raw = (PTE*)&cr3->Pte[(this->m_VirtAddr % kPTESize)];

  raw->Present = false;
  raw->Rw = false;
}

void PTEWrapper::Flush() {
  PDE* cr3 = (PDE*)hal_read_cr3();

  kcout << "CR3: " << hex_number((UIntPtr)cr3) << endl;
  kcout << "Index: " << hex_number((this->m_VirtAddr % kPTESize)) << endl;

  cr3->Pte[(this->m_VirtAddr % kPTESize)].Wt = m_Wt;
  cr3->Pte[(this->m_VirtAddr % kPTESize)].Rw = m_Rw;
  cr3->Pte[(this->m_VirtAddr % kPTESize)].Cache = m_Cache;
  cr3->Pte[(this->m_VirtAddr % kPTESize)].Present = m_Present;
  cr3->Pte[(this->m_VirtAddr % kPTESize)].ExecDisable = m_ExecDisable;

  kcout << "Wrote PTE to PDE: " << hex_number((UIntPtr)cr3) << endl;
}

void PageManager::FlushTLB(UIntPtr VirtAddr) {
  if (VirtAddr == kBadAddress) return;

  hal_flush_tlb(VirtAddr);
}

bool PTEWrapper::Reclaim() {
  if (!this->m_Present) {
    this->m_Present = true;
    return true;
  }

  return false;
}

PTEWrapper PageManager::Request(Boolean Rw, Boolean User,
                                 Boolean ExecDisable) {
  // Store PTE wrapper right after PTE.
  VoidPtr ptr = reinterpret_cast<PTEWrapper *>(
      HCore::HAL::hal_alloc_page(sizeof(PTEWrapper), Rw, User));


  return PTEWrapper{Rw, User, ExecDisable, (UIntPtr)ptr};
}

bool PageManager::Free(Ref<PTEWrapper *> &wrapper) {
  if (wrapper) {
    if (!Detail::page_disable(wrapper->VirtualAddress())) return false;
    return true;
  }

  return false;
}

const UIntPtr PTEWrapper::VirtualAddress() { return (m_VirtAddr + sizeof(PTE) + sizeof(PTEWrapper)); }

bool PTEWrapper::Shareable() { return m_Shareable; }

bool PTEWrapper::Present() { return m_Present; }

bool PTEWrapper::Access() { return m_Accessed; }

void PTEWrapper::NoExecute(const bool enable) { this->m_ExecDisable = enable; }

const bool &PTEWrapper::NoExecute() { return this->m_ExecDisable; }
}  // namespace HCore
