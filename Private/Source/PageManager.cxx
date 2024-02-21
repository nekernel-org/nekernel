/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/PageManager.hpp>

#ifdef __x86_64__
#include <HALKit/AMD64/HalPageAlloc.hpp>
#endif  // ifdef __x86_64__

//! null deref will throw (Page Zero detected, aborting program!)
#define kProtectedRegionEnd 512

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
  MUST_PASS(raw);

  raw->Present = false;
  raw->Rw = false;
}

void PTEWrapper::FlushTLB(Ref<PageManager> &pm) {
  volatile PTE *virtAddr = static_cast<volatile PTE *>(virtAddr);

  virtAddr->Present = this->m_Present;
  virtAddr->ExecDisable = this->m_ExecDisable;
  virtAddr->Rw = this->m_Rw;
  virtAddr->User = this->m_User;

  pm.Leak().FlushTLB(this->m_VirtAddr);
}

void PageManager::FlushTLB(UIntPtr VirtAddr) {
  if (VirtAddr == kBadAddress) return;

  flush_tlb(VirtAddr);
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
  PTEWrapper *PageTableEntry = reinterpret_cast<PTEWrapper *>(
      HCore::HAL::hal_alloc_page(sizeof(PTEWrapper), Rw, User));

  if (PageTableEntry == nullptr) {
    kcout << "PTEWrapper : Page table is nullptr!, ke_new_ke_heap failed!\n";
    return nullptr;
  }

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

////////////////////////////

// VIRTUAL ADDRESS

////////////////////////////

const UIntPtr &PTEWrapper::VirtualAddress() { return m_VirtAddr; }

////////////////////////////

// PAGE GETTERS

////////////////////////////

bool PTEWrapper::Shareable() { return m_Shareable; }

bool PTEWrapper::Present() { return m_Present; }

bool PTEWrapper::Access() { return m_Accessed; }

////////////////////////////

// NO EXECUTE PROTECTION

////////////////////////////

void PTEWrapper::NoExecute(const bool enable) { this->m_ExecDisable = enable; }
const bool &PTEWrapper::NoExecute() { return this->m_ExecDisable; }
}  // namespace HCore
