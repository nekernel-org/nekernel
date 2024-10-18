/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <NewKit/Utils.hxx>
#include <KernelKit/CodeMgr.hxx>
#include <KernelKit/UserProcessScheduler.hxx>

namespace Kernel
{
	/// @brief Executes a new process from a function. Kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	Bool sched_execute_thread(MainKind main, const Char* process_name) noexcept
	{
		if (!main)
			return No;

		UserProcess proc;
		proc.SetImageStart(reinterpret_cast<VoidPtr>(main));

		proc.Kind	   = UserProcess::kExectuableKind;
		proc.StackSize = kib_cast(32);

		rt_set_memory(proc.Name, 0, kProcessLen);
		rt_copy_memory((VoidPtr)process_name, proc.Name, rt_string_len(process_name));

		return UserProcessScheduler::The().Add(proc) > 0;
	}
} // namespace Kernel
