/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024-2025 Amlal El Mahrouss
 */

#include <user/SystemCalls.h>

/// @brief This program loads a code framework into Kernel's memory.

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  SCI_UNUSED(argc);
  SCI_UNUSED(argv);

  PrintOut(nullptr, "%s", "ld.fwrk: Framework Loader.\n");
  PrintOut(nullptr, "%s", "ld.fwrk: © 2024-2025 Amlal El Mahrouss, All rights reserved.\n");

  return EXIT_FAILURE;
}
