/* -------------------------------------------

	Copyright ZKA Technologies.

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
	bool sched_execute_thread(MainKind main, const Char* process_name) noexcept
	{
		if (!main)
			return No;

		UserProcess proc;
		proc.SetImageStart(reinterpret_cast<VoidPtr>(main));

		proc.Kind	   = UserProcess::kExeKind;
		proc.StackSize = mib_cast(4);

		rt_copy_memory((VoidPtr)process_name, proc.Name, rt_string_len(process_name));

		return UserProcessScheduler::The().Add(proc) != 0;
	}
} // namespace Kernel
