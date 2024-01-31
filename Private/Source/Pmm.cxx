/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Pmm.hpp>

namespace HCore {
Pmm::Pmm() = default;

Pmm::Pmm(Ref<PageManager *> &pm) : m_PageManager(pm) {
  MUST_PASS(pm.Leak());
  kcout << "[PMM] New PhysicalMemoryManager\r\n";
}

Pmm::~Pmm() = default;

/* If this returns Null pointer, enter emergency mode */
Ref<PTEWrapper *> Pmm::RequestPage(Boolean user, Boolean readWrite) {
  if (m_PageManager) {
    PTEWrapper *pt = m_PageManager.Leak()->Request(user, readWrite, true);

    if (pt) return Ref<PTEWrapper *>(pt);

    return {};
  }

  kcout << "[Pmm::RequestPage] Ref<PTEWrapper*> could not be created! "
           "m_PageManager is nullptr!\r\n";

  return {};
}

Boolean Pmm::FreePage(Ref<PTEWrapper *> PageRef) {
  if (!PageRef) return false;

  PageRef->m_Present = false;

  return true;
}

Boolean Pmm::TogglePresent(Ref<PTEWrapper *> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef->m_Present = Enable;

  return true;
}

Boolean Pmm::ToggleUser(Ref<PTEWrapper *> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef->m_Rw = Enable;

  return true;
}

Boolean Pmm::ToggleRw(Ref<PTEWrapper *> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef->m_Rw = Enable;

  return true;
}

Boolean Pmm::ToggleShare(Ref<PTEWrapper *> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef->m_Shareable = Enable;

  return true;
}
}  // namespace HCore
