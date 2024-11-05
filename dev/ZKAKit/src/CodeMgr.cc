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
		kcout << "Validating process...\r";

		if (!main)
			return No;

		UserProcess* proc = new UserProcess{reinterpret_cast<VoidPtr>(main)};

		kcout << "Setting-up process data...\r";

		proc->Kind		= UserProcess::kExectuableKind;
		proc->StackSize = kib_cast(4);

		rt_set_memory(proc->Name, 0, kProcessNameLen);
		rt_copy_memory((VoidPtr)process_name, proc->Name, rt_string_len(process_name));

		ProcessID id = UserProcessScheduler::The().Add(proc);

		delete proc;

		return id;
	}
} // namespace Kernel
