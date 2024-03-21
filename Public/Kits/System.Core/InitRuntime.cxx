/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Heap.hxx>

/// @brief Inits the process runtime
/// @return if it was succesful or not.
DWORD HcInitRuntime(VOID) {
  kInstanceObject = HcGetInstanceObject();
  CA_MUST_PASS(kInstanceObject);

  return 0;
}