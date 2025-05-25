#include <user/SystemCalls.h>

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  SCI_UNUSED(argc);

  if (argc < 2) {
    PrintOut(nullptr, "HELP: ping <hostname>\n");
    return EXIT_FAILURE;
  }

  PrintOut(nullptr, "Pinging %s...\n", argv[1]);

  for (SizeT i = 0U; i < 4; ++i) {
    SInt32 result = 0;

    if (result != 0) {
      PrintOut(nullptr, "Request timed out.\n");
      return EXIT_FAILURE;
    }

    SInt32 bytes = 64;              // Simulated response size
    SInt32 time  = 100 + (i * 10);  // Simulated response time
    SInt32 ttl   = 64;

    PrintOut(nullptr, "Reply from %s: bytes=%i time=%ims TTL=%i\n", argv[1], bytes, time, ttl);
  }

  return EXIT_SUCCESS;
}