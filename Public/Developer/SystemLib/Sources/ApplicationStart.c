/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Inits the system library.
/// @return if it was succesful or not.
CA_EXTERN_C VoidType __start(VoidType) {
  kSharedApplication = RtGetApp();
  CA_MUST_PASS(kSharedApplication);
}
