#include <libSystem/System.h>

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  SCI_UNUSED(argc);
  SCI_UNUSED(argv);

  if (argc < 2) {
    PrintOut(nullptr, "HELP: manual <tutorial_name>\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}