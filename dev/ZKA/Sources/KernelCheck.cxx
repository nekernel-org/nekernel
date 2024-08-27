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
#include <Modules/CoreCG/WindowRenderer.hxx>

#define SetMem(dst, byte, sz) Kernel::rt_set_memory((Kernel::VoidPtr)dst, byte, sz)
#define CopyMem(dst, src, sz) Kernel::rt_copy_memory((Kernel::VoidPtr)src, (Kernel::VoidPtr)dst, sz)
#define MoveMem(dst, src, sz) Kernel::rt_copy_memory((Kernel::VoidPtr)src, (Kernel::VoidPtr)dst, sz)

#define cWebsiteMacro "https://zka.nl/help"

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	void ke_stop(const Kernel::Int& id)
	{
		CGInit();

		auto panicTxt  = RGB(0xff, 0xff, 0xff);

		CG::CGDrawBackground();

		auto start_y = 10;
		auto x		 = 10;

		CGDrawString("newoskrnl.dll Stopped working properly so it had to stop.", start_y, x, panicTxt);

		CGFini();

		start_y += 10;

		// show text according to error id.

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			CGDrawString("0x00000008 Process execution fault, this is a catasrophic failure.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			CGDrawString("0x00000006 ACPI configuration error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_FILESYSTEM: {
			CGDrawString("0x0000000A Filesystem corruption error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			CGDrawString("0x00000000 Kernel heap pointer error, surely corrupted.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			CGDrawString("0x00000009 Undefined behavior error, image had to stop.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			CGDrawString("0x0000000A End of boot code, but nothing to continue.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			CGDrawString("0x00000005 Bad handshake error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_IPC: {
			CGDrawString("0x00000003 Bad kernel IPC error.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			CGDrawString("0x00000007 Kernel privilege violation.", start_y, x, panicTxt);
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			CGDrawString("0x0000000B Catasrophic kernel failure.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_FAILED: {
			CGDrawString("0x10000001 Assertion failed.", start_y, x, panicTxt);
			break;
		}
		default: {
			CGDrawString("0xFFFFFFFC Unknown kernel error.", start_y, x, panicTxt);
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
