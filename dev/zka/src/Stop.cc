/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <NewKit/Stop.h>

#include <ArchKit/ArchKit.h>
#include <KernelKit/Timer.h>
#include <KernelKit/DebugOutput.h>
#include <NewKit/String.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <modules/FB/FB.h>
#include <modules/FB/Text.h>

#define cWebsiteMacro "https://help.zws.com/"

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	void ke_stop(const Kernel::Int& id)
	{
		CGInit();

		auto panic_text = RGB(0xff, 0xff, 0xff);

		auto start_y = 10;
		auto x		 = 10;

		CGDrawString("minoskrnl.exe stopped working properly so it had to stop.", start_y, x, panic_text);
		start_y += 10;

		// simply offset from previous string and then write the website.
		CGDrawString("Please visit: ", start_y, x, panic_text);
		CGDrawString(cWebsiteMacro, start_y, x + (FONT_SIZE_X * rt_string_len("Please visit: ")), panic_text);

		CGFini();

		start_y += 10;

		// show text according to error id.

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			CGDrawString("0x00000008 Multi-Task error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			CGDrawString("0x00000006 ACPI configuration error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_PAGE: {
			CGDrawString("0x0000000B Write/Read in non paged area.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_FILESYSTEM: {
			CGDrawString("0x0000000A Filesystem error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			CGDrawString("0x00000000 Heap is corrupted.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			CGDrawString("0x00000009 Bad Behavior.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			CGDrawString("0x0000000A Boot code has finished executing, waiting for scheduler and other cores.", start_y, x, panic_text);
			return;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			CGDrawString("0x00000005 Handshake fault.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_IPC: {
			CGDrawString("0x00000003 Bad IPC/XPCOM message.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			CGDrawString("0x00000007 Privilege access violation.", start_y, x, panic_text);
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			CGDrawString("0x0000000B Unexpected violation.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM: {
			CGDrawString("0x10000001 Out of virtual memory.", start_y, x, panic_text);

			break;
		}
		case RUNTIME_CHECK_FAILED: {
			CGDrawString("0x10000001 Kernel Bug check appears to have failed, a dump has been written to the storage.", start_y, x, panic_text);
			break;
		}
		default: {
			CGDrawString("0xFFFFFFFC Unknown Kernel Error.", start_y, x, panic_text);
			break;
		}
		}
		};

		RecoveryFactory::Recover();
	}

	Void RecoveryFactory::Recover() noexcept
	{
		HAL::rt_halt();
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
