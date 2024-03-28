/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

#include <System.Core/Headers/Heap.h>

/// @brief Main Application object, retrieved from __start symbol.
CA_EXTERN_C ObjectRef kApplicationObject = NULL;

/// @brief Inits the library.
/// @return if it was succesful or not.
CA_EXTERN_C VoidType __start(VoidType) {
  kApplicationObject = RtGetAppObject();
  CA_MUST_PASS(kApplicationObject);
}
