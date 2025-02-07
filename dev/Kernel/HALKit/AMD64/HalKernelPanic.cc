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

		kout << "Kernel_Panic: " << message << endl;
		kout << "Kernel_Panic_ID: " << hex_number(id) << endl;
		kout << "Kernel_Panic_CR2:" << hex_number((UIntPtr)hal_read_cr2()) << endl;

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
			kout << "Kernel_Panic_File: " << file << endl;
			kout << "Kernel_Panic_Line: " << line << endl;

			ke_panic(RUNTIME_CHECK_FAILED, file); // Runtime Check failed
		}
	}
} // namespace Kernel
