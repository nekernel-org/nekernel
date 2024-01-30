/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/PageManager.hpp>

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
  if (VirtAddr == 0) {
    m_Wt = false;
    m_Rw = false;
    m_Cache = false;
    m_Shareable = false;
  }
}

PTEWrapper::~PTEWrapper() {
  PTE *raw = reinterpret_cast<PTE *>(m_VirtAddr);

  MUST_PASS(raw);
  MUST_PASS(!raw->Accessed);

  if (raw->Present) raw->Present = false;
}

void PTEWrapper::FlushTLB(Ref<PageManager> &pm) {
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
    kcout << "PTEWrapper : Page table is nullptr!, ke_new_ke_heap failed!";
    return nullptr;
  }

  *PageTableEntry =
      PTEWrapper{Rw, User, ExecDisable, Detail::create_page_wrapper(Rw, User)};
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

const UIntPtr &PTEWrapper::VirtualAddress() { return m_VirtAddr; }

bool PTEWrapper::Shareable() {
  auto raw = reinterpret_cast<PTE *>(m_VirtAddr);

  if (raw->Present) {
    m_Shareable = raw->Shared;
    return m_Shareable;
  } else {
    kcout << "[PTEWrapper::Shareable] page is not present!";
    return false;
  }
}

bool PTEWrapper::Present() {
  auto raw = reinterpret_cast<PTE *>(m_VirtAddr);

  if (raw->Present) {
    m_Present = raw->Present;
    return m_Present;
  } else {
    kcout << "[PTEWrapper::Present] page is not present!";
    return false;
  }
}

bool PTEWrapper::Access() {
  auto raw = reinterpret_cast<PTE *>(m_VirtAddr);

  if (raw->Present) {
    m_Accessed = raw->Accessed;
  }

  return m_Accessed;
}
}  // namespace HCore
