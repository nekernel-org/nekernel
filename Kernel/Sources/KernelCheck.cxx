/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>
#include <FirmwareKit/Handover.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>

EXTERN_C [[noreturn]] void ke_wait_for_debugger()
{
	while (Yes)
	{
#ifdef __NEWOS_AMD64__
		Kernel::HAL::rt_cli();
		Kernel::HAL::rt_halt();
#endif
	}
}

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	void ke_stop(const Kernel::Int& id)
	{
		kcout << "*** STOP *** \r";
		kcout << "*** Kernel has trigerred a runtime stop. *** \r";

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
			kcout << "*** WHAT: END OF CODE. *** \r";
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

		RecoveryFactory::Recover();

	}
	
	Void RecoveryFactory::Recover() noexcept
	{		
#ifdef __DEBUG__
		ke_wait_for_debugger();
#endif // ifdef __DEBUG__

		PowerFactoryInterface powerInterface(kHandoverHeader->f_HardwareTables.f_VendorPtr);
		powerInterface.Shutdown();
	}
	

	void ke_runtime_check(bool expr, const char* file, const char* line)
	{
		if (!expr)
		{
#ifdef __DEBUG__
			kcout << "newoskrnl: File: " << file << "\r";
			kcout << "newoskrnl: Line: " << line << "\r";

#endif // __DEBUG__

			Kernel::ke_stop(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace Kernel
