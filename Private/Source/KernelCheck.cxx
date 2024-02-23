/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>

extern "C" [[noreturn]] void ke_wait_for_debugger() {
  while (true) {
    HCore::HAL::rt_cli();
    HCore::HAL::rt_halt();
  }
}

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace HCore {
void ke_stop(const HCore::Int &id) {
  kcout << "*** STOP *** \r\n";
  kcout << "*** HCoreKrnl.exe has trigerred a runtime stop. *** \r\n";

  switch (id) {
    case RUNTIME_CHECK_PROCESS: {
      kcout << "*** CAUSE: RUNTIME_CHECK_PROCESS *** \r\n";
      break;
    }
    case RUNTIME_CHECK_ACPI: {
      kcout << "*** CAUSE: RUNTIME_CHECK_ACPI *** \r\n";
      break;
    }
    case RUNTIME_CHECK_POINTER: {
      kcout << "*** CAUSE: RUNTIME_CHECK_POINTER *** \r\n";
      break;
    }
    case RUNTIME_CHECK_BAD_BEHAVIOR: {
      kcout << "*** CAUSE: RUNTIME_CHECK_BAD_BEHAVIOR *** \r\n";
      break;
    }
    case RUNTIME_CHECK_BOOTSTRAP: {
      kcout << "*** CAUSE: RUNTIME_CHECK_BOOTSTRAP *** \r\n";
      break;
    }
    case RUNTIME_CHECK_HANDSHAKE: {
      kcout << "*** CAUSE: RUNTIME_CHECK_HANDSHAKE *** \r\n";
      break;
    }
    case RUNTIME_CHECK_LD: {
      kcout << "*** CAUSE: RUNTIME_CHECK_LD *** \r\n";
      break;
    }
    case RUNTIME_CHECK_INVALID_PRIVILEGE: {
      kcout << "*** CAUSE: RUNTIME_CHECK_INVALID_PRIVILEGE *** \r\n";
      break;
    }
  };

  DumpManager::Dump();

#ifdef __DEBUG__
  ke_wait_for_debugger();
#endif  // ifdef __DEBUG__
}

void ke_runtime_check(bool expr, const char *file, const char *line) {
  if (!expr) {
#ifdef __DEBUG__
    kcout << "Krnl: File: " << file << "\n";
    kcout << "Krnl: Line: " << line << "\n";

#endif  // __DEBUG__

    HCore::ke_stop(RUNTIME_CHECK_FAILED);  // Runtime Check failed
  }
}
}  // namespace HCore
