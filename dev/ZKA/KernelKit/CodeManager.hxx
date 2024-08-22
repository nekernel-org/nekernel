/* -------------------------------------------

	Copyright ZKA Technologies.

	File: CodeManager.hpp
	Purpose: Code Manager and Shared Objects.

	Revision History:

	30/01/24: Added file (amlel)
	3/8/24: Add UPP struct.

------------------------------------------- */

#pragma once

#include <KernelKit/PECodeManager.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/PEFSharedObject.hxx>

namespace Kernel
{
	/// @brief Main process entrypoint.
	typedef void (*MainKind)(void);

	/// @brief Executes a new process from a function. kernel code only.
	/// @note This sets up a new stack, anything on the main function that calls the kernel will not be accessible.
	/// @param main the start of the process.
	/// @return if the process was started or not.
	bool execute_from_image(MainKind main, const Char* processName) noexcept;
} // namespace Kernel
