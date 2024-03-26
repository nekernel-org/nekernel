/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Headers/Heap.hxx>

/// @brief Inits the DLL.
/// @return if it was succesful or not.
CA_EXTERN_C DWordType __DllMain(VoidType) {
  kApplicationObject = HcGetAppObject();
  CA_MUST_PASS(kApplicationObject);

  return 0;
}