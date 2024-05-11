/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief Process teams implementation.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hpp>

namespace NewOS
{
	/// @brief Process list array getter.
	/// @return The list of process to schedule.
	MutableArray<Ref<ProcessHeader>>& ProcessTeam::AsArray()
	{
		return mProcessList;
	}

	/// @brief Current process getter.
	/// @return The current process header.
	Ref<ProcessHeader>& ProcessTeam::AsRef()
	{
		return mCurrentProcess;
	}
} // namespace NewOS

// last rev 05-03-24
