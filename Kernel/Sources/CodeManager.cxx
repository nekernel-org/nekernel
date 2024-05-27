/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <NewKit/Utils.hpp>
#include <KernelKit/CodeManager.hpp>
#include <KernelKit/ProcessScheduler.hpp>

namespace NewOS
{
	/// @brief Executes a new process from a function. kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	bool execute_from_image(MainKind main, const char* processName) noexcept
	{
		if (!main)
			return false;

		ProcessHeader proc((VoidPtr)main);
		proc.Kind = ProcessHeader::kDriverKind;
		rt_copy_memory((VoidPtr)processName, proc.Name, rt_string_len(proc.Name));

		Ref<ProcessHeader> refProc = proc;

		return ProcessScheduler::The().Leak().Add(refProc);
	}
} // namespace NewOS