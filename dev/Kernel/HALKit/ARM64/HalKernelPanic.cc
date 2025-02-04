/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/KernelPanic.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/Timer.h>
#include <KernelKit/DebugOutput.h>
#include <NewKit/KString.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <Mod/CoreGfx/FBMgr.h>
#include <Mod/CoreGfx/TextMgr.h>
#include <NewKit/Utils.h>

/* Each error code is attributed with an ID, which will prompt a string onto the
 * screen. Wait for debugger... */

namespace Kernel
{
	/// @brief Dumping factory class.
	class RecoveryFactory final
	{
	public:
		STATIC Void Recover() noexcept;
	};

	/***********************************************************************************/
	/// @brief Stops execution of the kernel.
	/// @param id kernel stop ID.
	/***********************************************************************************/
	Void ke_panic(const Kernel::Int32& id, const Char* message)
	{
		fb_init();

		auto panic_text = RGB(0xff, 0xff, 0xff);

		auto y = 10;
		auto x = 10;

		Char* message_apicid = new Char[128];
		rt_set_memory(message_apicid, 0, 128);

		rt_copy_memory((VoidPtr) "panic id: ", message_apicid, rt_string_len("panic id: "));
		rt_to_string(message_apicid + rt_string_len("panic id: "), (UIntPtr)id, 10);

		fb_render_string(message_apicid, y, x, panic_text);

		y += 10;

		fb_render_string((message ? message : "message: panic raised, going nowhere after this!"), y, x, panic_text);

		y += 10;

		fb_clear();

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
			kout << "FAILED: FILE: " << file << endl;
			kout << "FAILED: LINE: " << line << endl;

			ke_panic(RUNTIME_CHECK_FAILED, file); // Runtime Check failed
		}
	}
} // namespace Kernel
