/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Heap.hxx>

/// @brief Inits the C runtime
/// @return if it was succesful or not.
DWORD HcInitRuntime(VOID) {
  kInstanceObject = HcGetProcessObject();
  return 0;
}