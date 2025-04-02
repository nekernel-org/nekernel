/* -------------------------------------------

	Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

	FILE: UserProcessScheduler.cc
	PURPOSE: Low level/Ring-3 Process scheduler.

------------------------------------------- */

#include <KernelKit/ProcessScheduler.h>

/***********************************************************************************/
/// @file KernelProcessScheduler.cc
/// @brief Privileged/Ring-0 process scheduler.
/// @author Amlal El Mahrouss (amlal@nekernel.org)
/***********************************************************************************/

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Exit Code global variable.
	/***********************************************************************************/

	STATIC UInt32 kLastExitCode = 0U;

} // namespace Kernel