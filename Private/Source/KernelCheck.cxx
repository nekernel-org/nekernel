/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>

extern "C" [[noreturn]] void ke_wait_for_debugger() {
  while (true) {
    NewOS::HAL::rt_cli();
    NewOS::HAL::rt_halt();
  }
}

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace NewOS {
void ke_stop(const NewOS::Int &id) {
  kcout << "*** STOP *** \r\n";
  kcout << "*** NewKernel.exe has trigerred a runtime stop. *** \r\n";

  switch (id) {
    case RUNTIME_CHECK_PROCESS: {
      kcout << "*** CAUSE: RUNTIME_CHECK_PROCESS *** \r\n";
      kcout << "*** WHAT: BAD DRIVER. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_ACPI: {
      kcout << "*** CAUSE: RUNTIME_CHECK_ACPI *** \r\n";
      kcout << "*** WHAT: ACPI ERROR, UNSTABLE STATE. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_POINTER: {
      kcout << "*** CAUSE: RUNTIME_CHECK_POINTER *** \r\n";
      kcout << "*** WHAT: HEAP CRC32 ERROR, UNSTABLE STATE. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_BAD_BEHAVIOR: {
      kcout << "*** CAUSE: RUNTIME_CHECK_BAD_BEHAVIOR *** \r\n";
      kcout << "*** WHAT: KERNEL BECAME UNSTABLE. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_BOOTSTRAP: {
      kcout << "*** CAUSE: RUNTIME_CHECK_BOOTSTRAP *** \r\n";
      kcout << "*** WHAT: INVALID BOOT SEQUENCE. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_HANDSHAKE: {
      kcout << "*** CAUSE: RUNTIME_CHECK_HANDSHAKE *** \r\n";
      kcout << "*** WHAT: BAD HANDSHAKE. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_IPC: {
      kcout << "*** CAUSE: RUNTIME_CHECK_IPC *** \r\n";
      kcout << "*** WHAT: RICH CALL VIOLATION. *** \r\n";
      break;
    }
    case RUNTIME_CHECK_INVALID_PRIVILEGE: {
      kcout << "*** CAUSE: RUNTIME_CHECK_INVALID_PRIVILEGE *** \r\n";
      kcout << "*** WHAT: RING-0 POLICY VIOLATION. *** \r\n";
      break;
    case RUNTIME_CHECK_UNEXCPECTED: {
      kcout << "*** CAUSE: RUNTIME_CHECK_UNEXCPECTED *** \r\n";
      kcout << "*** WHAT: CATASROPHIC FAILURE! *** \r\n";
      break;
    }
    case RUNTIME_CHECK_FAILED: {
      kcout << "*** CAUSE: RUNTIME_CHECK_FAILED *** \r\n";
      kcout << "*** WHAT: ASSERTION FAILED! *** \r\n";
      break;
    }
    default: {
      kcout << "*** CAUSE: RUNTIME_CHECK_GENERIC *** \r\n";
      break;
    }
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
    kcout << "NewKernel: File: " << file << "\n";
    kcout << "NewKernel: Line: " << line << "\n";

#endif  // __DEBUG__

    NewOS::ke_stop(RUNTIME_CHECK_FAILED);  // Runtime Check failed
  }
}
}  // namespace NewOS
