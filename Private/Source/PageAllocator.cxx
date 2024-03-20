/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/PageAllocator.hpp>

/// @brief Internal namespace, used internally by kernel.
namespace HCore::Detail {
VoidPtr create_page_wrapper(Boolean rw, Boolean user) {
  auto addr = HAL::hal_alloc_page(rw, user);

  if (addr == kBadAddress) {
    kcout << "[create_page_wrapper] kBadAddress returned\n";
    ke_stop(RUNTIME_CHECK_POINTER);
  }

  return addr;
}

void exec_disable(UIntPtr VirtualAddr) {
  PTE *VirtualAddrTable = reinterpret_cast<PTE *>(VirtualAddr);
  MUST_PASS(!VirtualAddrTable->Accessed);
  VirtualAddrTable->ExecDisable = true;

  hal_flush_tlb(VirtualAddr);
}

bool page_disable(UIntPtr VirtualAddr) {
  if (VirtualAddr) {
    auto VirtualAddrTable = (PTE *)(VirtualAddr);
    MUST_PASS(!VirtualAddrTable->Accessed);

    if (VirtualAddrTable->Accessed) return false;
    VirtualAddrTable->Present = false;

    hal_flush_tlb(VirtualAddr);

    return true;
  }

  return false;
}
}  // namespace HCore::Detail
