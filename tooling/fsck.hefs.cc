/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <tooling/hefs.h>
#include <tooling/mkfs.h>
#include <cstdlib>

int main(int argc, char** argv) {
  if (argc < 2) {
    mkfs::console_out() << "fsck: hefs: usage: fsck.hefs i <input_device>"
                        << "\n";
    return EXIT_FAILURE;
  }

  (void)(argv);

  return EXIT_SUCCESS;
}