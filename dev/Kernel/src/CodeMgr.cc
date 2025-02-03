/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/CodeMgr.h>
#include <NewKit/Utils.h>
#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Executes a new process from a function. Kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	/***********************************************************************************/

	ProcessID rtl_create_process(rtl_main_kind main, const Char* process_name) noexcept
	{
		if (!process_name ||
			*process_name == 0)
			return kProcessInvalidID;

		return UserProcessScheduler::The().Spawn(process_name, reinterpret_cast<VoidPtr>(main), nullptr);
	}
} // namespace Kernel
