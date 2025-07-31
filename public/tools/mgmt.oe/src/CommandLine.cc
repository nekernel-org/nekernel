/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <libSystem/SystemKit/Jail.h>
#include <libSystem/SystemKit/System.h>

/// @author Amlal El Mahrouss
/// @brief OpenEnclave management tool

static JAIL* kJailSrv = nullptr;

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  LIBSYS_UNUSED(argc);
  LIBSYS_UNUSED(argv);

  kJailSrv = JailGetCurrent();

  MUST_PASS(kJailSrv);

  PrintOut(nullptr, "%s", "mgmt.oe - OpenEnclave Management Tool.");

  /// @note JailGetCurrent returns client as nullptr if we're not that client (we'll not be able to access the jail then)
  if (kJailSrv->fClient == nullptr) return EXIT_FAILURE;

  return EXIT_FAILURE;
}