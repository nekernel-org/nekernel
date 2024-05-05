/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief Process teams.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hpp>

namespace NewOS
{
	/// @brief Process list array getter.
	/// @return
	MutableArray<Ref<ProcessHeader>>& ProcessTeam::AsArray()
	{
		return mProcessList;
	}

	/// @brief Current process getter.
	/// @return
	Ref<ProcessHeader>& ProcessTeam::AsRef()
	{
		return mCurrentProcess;
	}
} // namespace NewOS

// last rev 05-03-24
