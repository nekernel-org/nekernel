/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>

extern "C" [[noreturn]] void ke_wait_for_debugger()
{
	while (true)
	{
		NewOS::HAL::rt_cli();
		NewOS::HAL::rt_halt();
	}
}

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace NewOS
{
	void ke_stop(const NewOS::Int& id)
	{
		kcout << "*** STOP *** \r";
		kcout << "*** NewKernel.exe has trigerred a runtime stop. *** \r";

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			kcout << "*** CAUSE: RUNTIME_CHECK_PROCESS *** \r";
			kcout << "*** WHAT: BAD DRIVER. *** \r";
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			kcout << "*** CAUSE: RUNTIME_CHECK_ACPI *** \r";
			kcout << "*** WHAT: ACPI ERROR, UNSTABLE STATE. *** \r";
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			kcout << "*** CAUSE: RUNTIME_CHECK_POINTER *** \r";
			kcout << "*** WHAT: HEAP CRC32 ERROR, UNSTABLE STATE. *** \r";
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			kcout << "*** CAUSE: RUNTIME_CHECK_BAD_BEHAVIOR *** \r";
			kcout << "*** WHAT: KERNEL BECAME UNSTABLE. *** \r";
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			kcout << "*** CAUSE: RUNTIME_CHECK_BOOTSTRAP *** \r";
			kcout << "*** WHAT: INVALID BOOT SEQUENCE. *** \r";
			break;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			kcout << "*** CAUSE: RUNTIME_CHECK_HANDSHAKE *** \r";
			kcout << "*** WHAT: BAD HANDSHAKE. *** \r";
			break;
		}
		case RUNTIME_CHECK_IPC: {
			kcout << "*** CAUSE: RUNTIME_CHECK_IPC *** \r";
			kcout << "*** WHAT: RICH CALL VIOLATION. *** \r";
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			kcout << "*** CAUSE: RUNTIME_CHECK_INVALID_PRIVILEGE *** \r";
			kcout << "*** WHAT: HYPERVISOR POLICY VIOLATION. *** \r";
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			kcout << "*** CAUSE: RUNTIME_CHECK_UNEXCPECTED *** \r";
			kcout << "*** WHAT: CATASROPHIC FAILURE! *** \r";
			break;
		}
		case RUNTIME_CHECK_FAILED: {
			kcout << "*** CAUSE: RUNTIME_CHECK_FAILED *** \r";
			kcout << "*** WHAT: ASSERTION FAILED! *** \r";
			break;
		}
		default: {
			kcout << "*** CAUSE: RUNTIME_CHECK_GENERIC *** \r";
			break;
		}
		}
		};

		DumpManager::Dump();

#ifdef __DEBUG__
		ke_wait_for_debugger();
#endif // ifdef __DEBUG__
	}

	void ke_runtime_check(bool expr, const char* file, const char* line)
	{
		if (!expr)
		{
#ifdef __DEBUG__
			kcout << "New Kernel: File: " << file << "\r";
			kcout << "New Kernel: Line: " << line << "\r";

#endif // __DEBUG__

			NewOS::ke_stop(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace NewOS
