/* -------------------------------------------

	Copyright ZKA Technologies.

	File: CodeMgr.hpp
	Purpose: Code Mgr and DLL mgr.

	Revision History:

	30/01/24: Added file (amlel)
	3/8/24: Add UPP struct.

------------------------------------------- */

#pragma once

#include <KernelKit/PECodeMgr.hxx>
#include <KernelKit/PEFCodeMgr.hxx>
#include <KernelKit/IPEFDLLObject.hxx>

namespace Kernel
{
	/// @brief Main process entrypoint.
	typedef void (*MainKind)(void);

	/// @brief Executes a new process from a function. Kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the Kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	bool sched_execute_thread(MainKind main, const Char* process_name) noexcept;
} // namespace Kernel
