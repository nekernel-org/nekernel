/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief Process teams implementation.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hxx>

namespace Kernel
{
	/// @brief Process list array getter.
	/// @return The list of process to schedule.
	MutableArray<Ref<PROCESS_HEADER_BLOCK>>& ProcessTeam::AsArray()
	{
		return mProcessList;
	}

	/// @brief Get team ID.
	/// @return The team's ID.
	UInt64& ProcessTeam::Id() noexcept
	{
		return mTeamId;
	}

	/// @brief Current process getter.
	/// @return The current process header.
	Ref<PROCESS_HEADER_BLOCK>& ProcessTeam::AsRef()
	{
		return mCurrentProcess;
	}
} // namespace Kernel

// last rev 05-03-24
