/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Timer.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/String.hxx>
#include <FirmwareKit/Handover.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <KernelKit/FileMgr.hxx>
#include <Modules/CoreCG/Accessibility.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>
#include <Modules/CoreCG/WindowRenderer.hxx>

#define SetMem(dst, byte, sz) Kernel::rt_set_memory((Kernel::VoidPtr)dst, byte, sz)
#define CopyMem(dst, src, sz) Kernel::rt_copy_memory((Kernel::VoidPtr)src, (Kernel::VoidPtr)dst, sz)
#define MoveMem(dst, src, sz) Kernel::rt_copy_memory((Kernel::VoidPtr)src, (Kernel::VoidPtr)dst, sz)

#define cWebsiteMacro "https://help.el-mahrouss-logic.com/"

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	void ke_stop(const Kernel::Int& id)
	{
		CGInit();

		auto panicTxt = RGB(0xff, 0xff, 0xff);

		CG::CGDrawBackground();

		auto start_y = 10;
		auto x		 = 10;

		CGDrawString("newoskrnl.exe Stopped working properly so it had to stop.", start_y, x, panicTxt);
		start_y += 10;

		// simply offset from previous string and then write the website.
		CGDrawString("Please visit: ", start_y, x, panicTxt);
		CGDrawString(cWebsiteMacro, start_y, x + (FONT_SIZE_X * rt_string_len("Please visit: ")), panicTxt);

		CGFini();

		start_y += 10;

		// show text according to error id.

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			CGDrawString("0x00000008 No more processes to run, this is because that ZKA ran out of processes.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			CGDrawString("0x00000006 ACPI configuration error.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		}
		case RUNTIME_CHECK_PAGE: {
			CGDrawString("0x0000000B Write/Read in non paged area.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
		}
		case RUNTIME_CHECK_FILESYSTEM: {
			CGDrawString("0x0000000A Filesystem error.", start_y, x, panicTxt);

			PowerFactoryInterface power(nullptr);
			power.Shutdown();
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			CGDrawString("0x00000000 Heap error, the heap is corrupted.", start_y, x, panicTxt);

			PowerFactoryInterface power(nullptr);
			power.Shutdown();
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			CGDrawString("0x00000009 CPU access error.", start_y, x, panicTxt);

			PowerFactoryInterface power(nullptr);
			power.Shutdown();
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			CGDrawString("0x0000000A Boot Processor finished executing.", start_y, x, panicTxt);

			PowerFactoryInterface power(nullptr);
			power.Shutdown();
			break;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			CGDrawString("0x00000005 Handshake fault.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		}
		case RUNTIME_CHECK_IPC: {
			CGDrawString("0x00000003 Bad IPC/XPCOM message.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			CGDrawString("0x00000007 Privilege access violation.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			CGDrawString("0x0000000B Kernel access violation.", start_y, x, panicTxt);
			break;
		}
		case RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM: {
			CGDrawString("0x10000001 Out of virtual memory.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		}
		case RUNTIME_CHECK_FAILED: {
			CGDrawString("0x10000001 Kernel Bug check failed.", start_y, x, panicTxt);
			RecoveryFactory::Recover();
			break;
		}
		default: {
			RecoveryFactory::Recover();
			CGDrawString("0xFFFFFFFC Unknown Kernel error.", start_y, x, panicTxt);
			break;
		}
		}
		};

		PowerFactoryInterface power(nullptr);
		power.Reboot();
	}

	Void RecoveryFactory::Recover() noexcept
	{
		PowerFactoryInterface power(nullptr);
		power.Reboot();
	}

	void ke_runtime_check(bool expr, const Char* file, const Char* line)
	{
		if (!expr)
		{
			kcout << "FAILED: FILE: " << file << endl;
			kcout << "FAILED: LINE: " << line << endl;

			ke_stop(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace Kernel
