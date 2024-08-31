/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <NewKit/Utils.hxx>
#include <KernelKit/CodeManager.hxx>
#include <KernelKit/UserProcessScheduler.hxx>

namespace Kernel
{
	/// @brief Executes a new process from a function. Kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	bool execute_from_image(MainKind main, const Char* processName) noexcept
	{
		if (!main)
			return false;

		UserProcess proc((VoidPtr)main);
		
		if (mm_is_valid_heap((VoidPtr)main))
			proc.SetEntrypoint(reinterpret_cast<VoidPtr>(main));

		proc.Kind = UserProcess::kExeKind;
		proc.StackSize = mib_cast(1);

		rt_copy_memory((VoidPtr)processName, proc.Name, rt_string_len(processName));

		return UserProcessScheduler::The().Add(proc) == kErrorSuccess;
	}
} // namespace Kernel
