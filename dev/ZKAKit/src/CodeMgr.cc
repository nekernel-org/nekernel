/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <KernelKit/CodeMgr.h>
#include <NewKit/Utils.h>
#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	/// @brief Executes a new process from a function. Kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	ProcessID rtl_create_process(MainKind main, const Char* process_name) noexcept
	{
		if (!main)
			return No;

		UserProcess proc;

		proc.Image	   = reinterpret_cast<VoidPtr>(main);
		proc.Kind	   = UserProcess::kExectuableKind;
		proc.StackSize = kib_cast(32);

		rt_set_memory(proc.Name, 0, kProcessLen);
		rt_copy_memory((VoidPtr)process_name, proc.Name, rt_string_len(process_name));

		return UserProcessScheduler::The().Add(proc);
	}
} // namespace Kernel
