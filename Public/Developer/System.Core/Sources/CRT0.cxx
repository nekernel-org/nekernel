/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Headers/Heap.hxx>

/// @brief Inits the library.
/// @return if it was succesful or not.
CA_EXTERN_C DWordType __start(VoidType) {
  kApplicationObject = HcGetAppObject();
  CA_MUST_PASS(kApplicationObject);

  return 0;
}