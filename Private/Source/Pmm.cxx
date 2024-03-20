/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Pmm.hpp>

namespace HCore {
Pmm::Pmm() : m_PageManager() { kcout << "[PMM] Allocate PageMemoryManager"; }

Pmm::~Pmm() = default;

/* If this returns Null pointer, enter emergency mode */
Ref<PTEWrapper> Pmm::RequestPage(Boolean user, Boolean readWrite) {
  PTEWrapper pt = m_PageManager.Leak().Request(user, readWrite, false);

  if (pt.m_Present) {
    kcout << "[PMM]: Allocation was successful.";
    return Ref<PTEWrapper>(pt);
  }

  kcout << "[PMM]: Allocation failure.";

  return {};
}

Boolean Pmm::FreePage(Ref<PTEWrapper> PageRef) {
  if (!PageRef) return false;

  PageRef.Leak().m_Present = false;
  PageRef.Leak().Flush();

  return true;
}

Boolean Pmm::TogglePresent(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().m_Present = Enable;

  return true;
}

Boolean Pmm::ToggleUser(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().m_Rw = Enable;

  return true;
}

Boolean Pmm::ToggleRw(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().m_Rw = Enable;

  return true;
}

Boolean Pmm::ToggleShare(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().m_Shareable = Enable;

  return true;
}
}  // namespace HCore
