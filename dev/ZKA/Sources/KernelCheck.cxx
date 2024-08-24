/* -------------------------------------------

	Copyright ZKA Technologies.

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

#define cWebsiteMacro "https://zka.nl/help"

#include <BootKit/Vendor/Qr.hxx>

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	void ke_stop(const Kernel::Int& id)
	{
		CGInit();

		auto panicBack = RGB(0xff, 0x3a, 0x3a);
		auto panicTxt  = RGB(0xff, 0xff, 0xff);

		CGDrawInRegion(panicBack, CG::UIAccessibilty::The().Height(), CG::UIAccessibilty::The().Width(), 0, 0);

		auto start_y = 10;
		auto x		 = 10;

		CGDrawString("newoskrnl.dll stopped working properly so we had to stop.", start_y, x, panicTxt);

		CGFini();

		// Show the QR code now.

		constexpr auto cVer		 = 4;
		const auto	   cECC		 = qr::Ecc::H;
		const auto	   cInput	 = cWebsiteMacro;
		const auto	   cInputLen = rt_string_len(cWebsiteMacro);

		qr::Qr<cVer>   encoder;
		qr::QrDelegate encoderDelegate;

		encoder.encode(cInput, cInputLen, cECC, 0); // Manual mask 0

		const auto cWhereStartX = (kHandoverHeader->f_GOP.f_Width - encoder.side_size()) - 20;
		const auto cWhereStartY = (kHandoverHeader->f_GOP.f_Height - encoder.side_size()) / 2;

		// tell delegate to draw encoded QR now.
		encoderDelegate.draw<cVer>(encoder, cWhereStartX,
								   cWhereStartY);

		start_y += 10;

		// show text according to error id.

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			CGDrawString("0x00000008 Process scheduler error (Catasrophic failure).", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			CGDrawString("0x00000006 ACPI error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_FILESYSTEM: {
			CGDrawString("0x0000000A Filesystem corruption error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			CGDrawString("0x00000000 Kernel heap error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			CGDrawString("0x00000009 Undefined Behavior error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			CGDrawString("0x0000000A End of code.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			CGDrawString("0x00000005 Handshake error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_IPC: {
			CGDrawString("0x00000003 Kernel IPC error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			CGDrawString("0x00000007 Kernel privilege violation.", start_y, x, panicTxt);
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			CGDrawString("0x0000000B Catasrophic failure.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_FAILED: {
			CGDrawString("0x10000001 Assertion failed.", start_y, x, panicTxt);
			break;
		}
		default: {
			CGDrawString("0xFFFFFFFF Unknown error.", start_y, x, panicTxt);
			break;
		}
		}
		};

		RecoveryFactory::Recover();
	}

	Void RecoveryFactory::Recover() noexcept
	{
		while (Yes)
		{
			asm volatile("cli; hlt");
		}
	}

	void ke_runtime_check(bool expr, const Char* file, const Char* line)
	{
		if (!expr)
		{
			ke_stop(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace Kernel
