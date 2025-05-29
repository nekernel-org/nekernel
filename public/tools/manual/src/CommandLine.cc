#include <libSystem/System.h>

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  LIBSYS_UNUSED(argc);
  LIBSYS_UNUSED(argv);

  if (argc < 2) {
    PrintOut(nullptr, "HELP: manual <tutorial_name>\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}