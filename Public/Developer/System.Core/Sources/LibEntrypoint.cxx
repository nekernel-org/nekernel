/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Headers/Heap.hxx>

/// @brief Inits the DLL.
/// @return if it was succesful or not.
DWordType __DllMain(VoidType) {
  kApplicationObject = HcGetInstanceObject();
  CA_MUST_PASS(kApplicationObject);

  return 0;
}