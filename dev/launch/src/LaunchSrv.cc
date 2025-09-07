/* -------------------------------------------

   Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

   ------------------------------------------- */

#include <LaunchKit/LaunchKit.h>

SInt32 _NeMain(Void) {
  PrintOut(nullptr, "%s", "NeKernel Launcher\n");

  /// @todo Start LaunchServices.fwrk, make the launcher manageable too (via mgmt.launch)

  return kErrorSuccess;
}
