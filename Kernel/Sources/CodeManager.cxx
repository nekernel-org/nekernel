/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <NewKit/Utils.hxx>
#include <KernelKit/CodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>

namespace Kernel
{
	/// @brief Executes a new process from a function. kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	bool execute_from_image(MainKind main, const char* processName) noexcept
	{
		if (!main)
			return false;

		PROCESS_HEADER_BLOCK proc((VoidPtr)main);
		proc.Kind = PROCESS_HEADER_BLOCK::kAppKind;
		rt_copy_memory((VoidPtr)processName, proc.Name, rt_string_len(proc.Name));

		Ref<PROCESS_HEADER_BLOCK> refProc = proc;

		return ProcessScheduler::The().Leak().Add(refProc);
	}
} // namespace Kernel
