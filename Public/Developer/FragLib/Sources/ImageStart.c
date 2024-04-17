/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Application entrypoint.
/// @param void
/// @return void
CA_EXTERN_C VoidType AppMain(VoidType);

/// @brief Process entrypoint.
/// @param void
/// @return void
CA_EXTERN_C VoidType __ImageStart(VoidType) {
  kSharedApplication = RtGetAppPointer();
  CA_MUST_PASS(kSharedApplication);

  AppMain();
}
