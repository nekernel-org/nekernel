/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/String.hxx>
#include <FirmwareKit/Handover.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>

#include <Modules/CoreCG/Accessibility.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>

#define SetMem(dst, byte, sz) Kernel::rt_set_memory((Kernel::VoidPtr)dst, byte, sz)
#define CopyMem(dst, src, sz) Kernel::rt_copy_memory((Kernel::VoidPtr)src, (Kernel::VoidPtr)dst, sz)
#define MoveMem(dst, src, sz) Kernel::rt_copy_memory((Kernel::VoidPtr)src, (Kernel::VoidPtr)dst, sz)

#define cWebsiteMacro "https://zka.ma/help"

#include <BootKit/Vendor/Qr.hxx>

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	void ke_stop(const Kernel::Int& id)
	{
		CGInit();

		auto panicBack = RGB(0xDC, 0xF5, 0xF5);
		auto panicTxt = RGB(0, 0, 0);
		
		CGDrawInRegion(panicBack, UIAccessibilty::The().Height(), UIAccessibilty::The().Width(), 0, 0);

		auto start_y = 10;
		auto x = 10;

		cg_write_text("*** Kernel panic! ***\rnewoskrnl.dll stopped working properly so we had to shut it down.", start_y, x, panicTxt);

		CGFini();

		// Show the QR code now.

		constexpr auto cVer     = 4;
		const auto		   cECC = qr::Ecc::H;
		const auto		   cInput = cWebsiteMacro;
		const auto		   cInputLen = rt_string_len(cWebsiteMacro);

		qr::Qr<cVer>	   encoder;
		qr::QrDelegate     encoderDelegate;

		encoder.encode(cInput, cInputLen, cECC, 0); // Manual mask 0

		const auto cWhereStartX = (kHandoverHeader->f_GOP.f_Width - encoder.side_size()) - 20;
		const auto cWhereStartY = (kHandoverHeader->f_GOP.f_Height - encoder.side_size()) / 2;

		// tell delegate to draw encoded QR now.
		encoderDelegate.draw<cVer>(encoder, cWhereStartX,
								  cWhereStartY);

		// ******* //
		// shows in debug only.
		// ******* //

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
		while (true)
		{
			asm volatile ("cli; hlt");
		}
	}
	

	void ke_runtime_check(bool expr, const char* file, const char* line)
	{
		if (!expr)
		{
			ke_stop(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace Kernel
