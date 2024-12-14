/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#include <NewKit/Stop.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/Timer.h>
#include <KernelKit/DebugOutput.h>
#include <NewKit/KString.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <Modules/FB/FB.h>
#include <Modules/FB/Text.h>

#define kWebsiteURL "https://www.el-mahrouss-logic.com/metal-os/help/"

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Stops execution of the kernel.
	/// @param id kernel stop ID.
	/***********************************************************************************/
	Void ke_stop(const Kernel::Int32& id)
	{
		cg_init();

		auto panic_text = RGB(0xff, 0xff, 0xff);

		auto start_y = 50;
		auto x		 = 10;

		if (id != RUNTIME_CHECK_BOOTSTRAP)
			CGDrawString("Kernel Panic!", start_y, x, panic_text);
		else
			CGDrawString("Kernel Splash:", start_y, x, panic_text);
		start_y += 10;

		cg_fini();

		// show text according to error id.

		switch (id)
		{
		case RUNTIME_CHECK_PROCESS: {
			CGDrawString("0x00000008: Invalid process behavior.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_ACPI: {
			CGDrawString("0x00000006: ACPI configuration error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_PAGE: {
			CGDrawString("0x0000000B: Write/Read in non paged area.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_FILESYSTEM: {
			CGDrawString("0x0000000A: Filesystem driver error.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_POINTER: {
			CGDrawString("0x00000000: Kernel heap is corrupted.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_BAD_BEHAVIOR: {
			CGDrawString("0x00000009: Bad behavior.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_BOOTSTRAP: {
			CGDrawString("0x0000000A: Boot Code has finished running, running Kernel process...", start_y, x, panic_text);
			return;
		}
		case RUNTIME_CHECK_HANDSHAKE: {
			CGDrawString("0x00000005: Handshake fault.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_IPC: {
			CGDrawString("0x00000003: Bad LPC message.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_INVALID_PRIVILEGE: {
			CGDrawString("0x00000007: Privilege access violation.", start_y, x, panic_text);
			break;
		case RUNTIME_CHECK_UNEXCPECTED: {
			CGDrawString("0x0000000B: Unexpected violation.", start_y, x, panic_text);
			break;
		}
		case RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM: {
			CGDrawString("0x10000001: Out of virtual memory.", start_y, x, panic_text);

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

			ke_stop(RUNTIME_CHECK_FAILED); // Runtime Check failed
		}
	}
} // namespace Kernel
