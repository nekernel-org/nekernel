/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024-2025 Amlal El Mahrouss
 */

#include <libSystem/SystemKit/System.h>

/// @brief Library loader.

#define DYNLIB_FLAG "-dyn"

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  LIBSYS_UNUSED(argc);
  LIBSYS_UNUSED(argv);

  PrintOut(nullptr, "%s", "ld.dyn: Dynamic Loader.\n");
  PrintOut(nullptr, "%s", "ld.dyn: © 2024-2025 Amlal El Mahrouss, All rights reserved.\n");

  for (SInt32 i = 1U; i < argc; ++i) {
    if (MmStrCmp(argv[i], DYNLIB_FLAG) == 0) {
      UIntPtr ret = RtlSpawnProcess(argv[i], 0, nullptr, nullptr, 0);

      if (0 < ret) {
        return RtlSpawnIB(ret);
      }

      PrintOut(nullptr, "%s", "ld.dyn: Failed to load the library.\n");
      PrintOut(nullptr, "%s", "ld.dyn: Make sure the library is valid.\n");

      break;
    } else {
      PrintOut(nullptr, "%s", "ld.dyn: Invalid argument.\n");
      PrintOut(nullptr, "%s", "ld.dyn: Use -dyn <path> to load a dynamic library.\n");

      break;
    }
  }

  return EXIT_FAILURE;
}
