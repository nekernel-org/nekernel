/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief UserProcess teams implementation.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.hxx>

namespace Kernel
{
	/// @brief UserProcess list array getter.
	/// @return The list of process to schedule.
	Array<UserProcess, kSchedProcessLimitPerTeam>& ProcessTeam::AsArray()
	{
		return mProcessList;
	}

	/// @brief Get team ID.
	/// @return The team's ID.
	ProcessID& ProcessTeam::Id() noexcept
	{
		return mTeamId;
	}

	/// @brief Current process getter.
	/// @return The current process header.
	Ref<UserProcess>& ProcessTeam::AsRef()
	{
		return mCurrentProcess;
	}
} // namespace Kernel

// last rev 05-03-24
