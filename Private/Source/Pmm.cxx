/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Pmm.hpp>

namespace NewOS {
Pmm::Pmm() : fPageManager() { kcout << "[PMM] Allocate PageMemoryManager"; }

Pmm::~Pmm() = default;

/* If this returns Null pointer, enter emergency mode */
Ref<PTEWrapper> Pmm::RequestPage(Boolean user, Boolean readWrite) {
  PTEWrapper pt = fPageManager.Leak().Request(user, readWrite, false);

  if (pt.fPresent) {
    kcout << "[PMM]: Allocation was successful.";
    return Ref<PTEWrapper>(pt);
  }

  kcout << "[PMM]: Allocation failure.";

  return {};
}

Boolean Pmm::FreePage(Ref<PTEWrapper> PageRef) {
  if (!PageRef) return false;

  PageRef.Leak().fPresent = false;

  return true;
}

Boolean Pmm::TogglePresent(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().fPresent = Enable;

  return true;
}

Boolean Pmm::ToggleUser(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().fRw = Enable;

  return true;
}

Boolean Pmm::ToggleRw(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().fRw = Enable;

  return true;
}

Boolean Pmm::ToggleShare(Ref<PTEWrapper> PageRef, Boolean Enable) {
  if (!PageRef) return false;

  PageRef.Leak().fShareable = Enable;

  return true;
}
}  // namespace NewOS
