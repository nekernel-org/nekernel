/* -------------------------------------------

   Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

   ------------------------------------------- */

#include <LaunchKit/LaunchKit.h>
#include <libSystem/SystemKit/Err.h>

SInt32 _NeMain(Void) {
  /// @todo Start LaunchServices.fwrk services, make the launcher manageable too (via mgmt.launch)

  NELAUNCH_INFO("Starting services...");

  return kErrorSuccess;
}
