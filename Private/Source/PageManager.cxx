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
  PTE *raw = reinterpret_cast<PTE *>(m_VirtAddr);

  raw->Present = false;
  raw->Rw = false;
}

void PTEWrapper::FlushTLB(Ref<PageManager> &pm) {
  PTE* ptIndex = (PTE*)((m_VirtAddr >> 12) & 0x1FF);
  ptIndex->Wt = m_Wt;
  ptIndex->Rw = m_Rw;
  ptIndex->Cache = m_Cache;
  ptIndex->Present = m_Present;
  ptIndex->ExecDisable = m_ExecDisable;

  pm.Leak().FlushTLB((UIntPtr)ptIndex);
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

PTEWrapper *PageManager::Request(Boolean Rw, Boolean User,
                                 Boolean ExecDisable) {
  // Store PTE wrapper right after PTE.
  PTEWrapper *PageTableEntry = reinterpret_cast<PTEWrapper *>(
      HCore::HAL::hal_alloc_page(sizeof(PTEWrapper), Rw, User) + sizeof(PTE));

  PageTableEntry->NoExecute(ExecDisable);

  *PageTableEntry = PTEWrapper{Rw, User, ExecDisable,
                               reinterpret_cast<UIntPtr>(PageTableEntry)};
  return PageTableEntry;
}

bool PageManager::Free(Ref<PTEWrapper *> &wrapper) {
  if (wrapper) {
    if (!Detail::page_disable(wrapper->VirtualAddress())) return false;

    this->FlushTLB(wrapper->VirtualAddress());
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
