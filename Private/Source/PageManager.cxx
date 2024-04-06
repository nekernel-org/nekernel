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

namespace NewOS {
PTEWrapper::PTEWrapper(Boolean Rw, Boolean User, Boolean ExecDisable,
                       UIntPtr VirtAddr)
    : fRw(Rw),
      fUser(User),
      fExecDisable(ExecDisable),
      fVirtAddr(VirtAddr),
      fCache(false),
      fShareable(false),
      fWt(false),
      fPresent(true),
      fAccessed(false) {}

PTEWrapper::~PTEWrapper() {}

void PageManager::FlushTLB(UIntPtr VirtAddr) {
  if (VirtAddr == kBadAddress) return;

  hal_flush_tlb(VirtAddr);
}

bool PTEWrapper::Reclaim() {
  if (!this->fPresent) {
    this->fPresent = true;
    return true;
  }

  return false;
}

PTEWrapper PageManager::Request(Boolean Rw, Boolean User, Boolean ExecDisable) {
  // Store PTE wrapper right after PTE.
  VoidPtr ptr = NewOS::HAL::hal_alloc_page(Rw, User);

  return PTEWrapper{Rw, User, ExecDisable, (UIntPtr)ptr};
}

bool PageManager::Free(Ref<PTEWrapper *> &wrapper) {
  if (wrapper) {
    if (!Detail::page_disable(wrapper->VirtualAddress())) return false;
    return true;
  }

  return false;
}

const UIntPtr PTEWrapper::VirtualAddress() {
  return (fVirtAddr + sizeof(PTE) + sizeof(PTEWrapper));
}

bool PTEWrapper::Shareable() { return fShareable; }

bool PTEWrapper::Present() { return fPresent; }

bool PTEWrapper::Access() { return fAccessed; }

void PTEWrapper::NoExecute(const bool enable) { this->fExecDisable = enable; }

const bool &PTEWrapper::NoExecute() { return this->fExecDisable; }
}  // namespace NewOS
