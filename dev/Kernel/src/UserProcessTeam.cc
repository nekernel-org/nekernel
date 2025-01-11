/* -------------------------------------------

	Copyright (C) 2024, t& Corporation, all rights reserved.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessTeam.cc
/// @brief Process teams implementation.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.h>

namespace Kernel
{
	UserProcessTeam::UserProcessTeam()
	{
		for (SizeT i = 0U; i < this->mProcessList.Count(); ++i)
		{
			this->mProcessList[i]		 = UserThread();
			this->mProcessList[i].Status = ProcessStatusKind::kKilled;
		}

		this->mProcessCount = 0UL;
	}

	/***********************************************************************************/
	/// @brief UserThread list array getter.
	/// @return The list of process to schedule.
	/***********************************************************************************/

	Array<UserThread, kSchedProcessLimitPerTeam>& UserProcessTeam::AsArray()
	{
		return this->mProcessList;
	}

	/***********************************************************************************/
	/// @brief Get team ID.
	/// @return The team's ID.
	/***********************************************************************************/

	ProcessID& UserProcessTeam::Id() noexcept
	{
		return this->mTeamId;
	}

	/***********************************************************************************/
	/// @brief Get current process getter as Ref.
	/// @return The current process header.
	/***********************************************************************************/

	Ref<UserThread>& UserProcessTeam::AsRef()
	{
		return this->mCurrentProcess;
	}
} // namespace Kernel

// last rev 05-03-24
