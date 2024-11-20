/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

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
		kcout << "Validating process header...\r";

		if (!main)
			return No;

		UserProcess* process_hdr = new UserProcess();

		kcout << "Setting-up process data...\r";

		process_hdr->Code	   = reinterpret_cast<VoidPtr>(main);
		process_hdr->Kind	   = UserProcess::kExectuableKind;
		process_hdr->StackSize = kib_cast(16);

		rt_set_memory(process_hdr->Name, 0, kProcessNameLen);

		ProcessID id = UserProcessScheduler::The().Spawn(process_hdr);

		delete process_hdr;

		return id;
	}
} // namespace Kernel
