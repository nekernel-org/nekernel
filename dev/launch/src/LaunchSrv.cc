/* -------------------------------------------

   Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

   ------------------------------------------- */

#include <LaunchKit/LaunchKit.h>
#include <libSystem/SystemKit/Err.h>

/// @note This called by _NeMain from its own runtime.
extern "C" SInt32 nelaunch_startup_fn(Void) {
  /// @todo Start LaunchServices.fwrk services, make the launcher manageable too (via mgmt.launch)

  NELAUNCH_INFO("Starting NeKernel services...");

  return kErrorSuccess;
}
