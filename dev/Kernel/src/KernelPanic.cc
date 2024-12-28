/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#include <NewKit/KernelPanic.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/Timer.h>
#include <KernelKit/DebugOutput.h>
#include <NewKit/KString.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <Mod/GfxMgr/FBMgr.h>
#include <Mod/GfxMgr/TextMgr.h>

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Stops execution of the kernel.
	/// @param id kernel stop ID.
	/***********************************************************************************/
	Void ke_panic(const Kernel::Int32& id)
	{
		fb_init();

		auto panic_text = RGB(0xff, 0xff, 0xff);

		auto start_y = 50;
		auto x		 = 10;

		if (id != RUNTIME_CHECK_BOOTSTRAP)
			fb_render_string("Kernel Panic!", start_y, x, panic_text);
		else
			fb_render_string("Kernel Bootstrap:", start_y, x, panic_text);

		start_y += 10;

		fb_clear();

		// show text according to error id.

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			fb_render_string("0x00000008: Invalid process behavior.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			fb_render_string("0x00000006: ACPI configuration error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_PAGE: {
			fb_render_string("0x0000000B: Write/Read in non paged area.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_FILESYSTEM: {
			fb_render_string("0x0000000A: Filesystem driver error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			fb_render_string("0x00000000: Kernel heap is corrupted.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			fb_render_string("0x00000009: Bad behavior.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			fb_render_string("0x0000000A: Kernel has finished running, running OSLdr...", start_y, x, panic_text);
			return;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			fb_render_string("0x00000005: Handshake fault.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_IPC: {
			fb_render_string("0x00000003: Bad LPC message.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			fb_render_string("0x00000007: Privilege access violation.", start_y, x, panic_text);
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			fb_render_string("0x0000000B: Unexpected violation.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM: {
			fb_render_string("0x10000001: Out of virtual memory.", start_y, x, panic_text);

			break;
		}
		case RUNTIME_CHECK_FAILED: {
			fb_render_string("0x10000001: Kernel Bug check appears to have failed, a dump has been written to the storage.", start_y, x, panic_text);
			break;
		}
		default: {
			fb_render_string("0xFFFFFFFC: Unknown Kernel Error code.", start_y, x, panic_text);
			break;
		}
		}
		};

		RecoveryFactory::Recover();
	}

	Void RecoveryFactory::Recover() noexcept
	{
		while (YES)
		{
			HAL::rt_halt();
		}
	}

	void ke_runtime_check(bool expr, const Char* file, const Char* line)
	{
		if (!expr)
		{
			kcout << "FAILED: FILE: " << file << endl;
			kcout << "FAILED: LINE: " << line << endl;

			ke_panic(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace Kernel
