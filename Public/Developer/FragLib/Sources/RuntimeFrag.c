/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Defines.h>

/// @brief Main application entrypoint.
/// @param  
/// @return 
CA_EXTERN_C VoidType AppMain(VoidType);

/// @brief Pre-entrypoint intiialization.
/// @param  
/// @return 
CA_EXTERN_C VoidType __start(VoidType) {
  kSharedApplication = RtGetApp();
  CA_MUST_PASS(kSharedApplication);

  AppMain();
}
