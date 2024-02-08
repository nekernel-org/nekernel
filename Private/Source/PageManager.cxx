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

////////////////////////////

// VIRTUAL ADDRESS

////////////////////////////

const UIntPtr &PTEWrapper::VirtualAddress() { return m_VirtAddr; }

////////////////////////////

// PAGE GETTERS

////////////////////////////

bool PTEWrapper::Shareable() {
  auto raw = reinterpret_cast<PTE *>(m_VirtAddr);

  if (raw->Present) {
    m_Shareable = raw->Rw;
    kcout << m_Shareable ? "[PTEWrapper::Shareable] page is sharable!\n"
                         : "[PTEWrapper::Shareable] page is not sharable!\n";

    return m_Shareable;
  } else {
    kcout << "[PTEWrapper::Shareable] page is not present!\n";
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

////////////////////////////

// NO EXECUTE PROTECTION

////////////////////////////

void PTEWrapper::NoExecute(const bool enable) { this->m_ExecDisable = enable; }
const bool &PTEWrapper::NoExecute() { return this->m_ExecDisable; }
}  // namespace HCore
