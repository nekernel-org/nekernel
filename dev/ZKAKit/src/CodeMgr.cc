/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

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
		if (*process_name == 0)
			return kProcessInvalidID;

		UserProcess* process_hdr = new UserProcess();

		process_hdr->Image.fCode = reinterpret_cast<VoidPtr>(main);
		process_hdr->Kind		 = UserProcess::kExectuableKind;
		process_hdr->StackSize	 = kib_cast(8);

		rt_set_memory(process_hdr->Name, 0, kProcessNameLen);
		rt_copy_memory((VoidPtr)process_name, process_hdr->Name, rt_string_len(process_name));

		ProcessID id = UserProcessScheduler::The().Spawn(process_hdr);

		if (id == kProcessInvalidID)
			delete process_hdr;

		return id;
	}
} // namespace Kernel
