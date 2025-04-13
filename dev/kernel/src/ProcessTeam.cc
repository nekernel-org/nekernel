/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cc
/// @brief Process teams implementation.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.h>

namespace Kernel
{
	ProcessTeam::ProcessTeam()
	{
		for (SizeT i = 0U; i < this->mProcessList.Count(); ++i)
		{
			this->mProcessList[i]		 = Process();
			this->mProcessList[i].PTime	 = 0;
			this->mProcessList[i].Status = ProcessStatusKind::kKilled;
		}

		this->mProcessCount = 0UL;
	}

	/***********************************************************************************/
	/// @brief Process list array getter.
	/// @return The list of process to schedule.
	/***********************************************************************************/

	Array<Process, kSchedProcessLimitPerTeam>& ProcessTeam::AsArray()
	{
		return this->mProcessList;
	}

	/***********************************************************************************/
	/// @brief Get team ID.
	/// @return The team's ID.
	/***********************************************************************************/

	ProcessID& ProcessTeam::Id() noexcept
	{
		return this->mTeamId;
	}

	/***********************************************************************************/
	/// @brief Get current process getter as Ref.
	/// @return The current process header.
	/***********************************************************************************/

	Ref<Process>& ProcessTeam::AsRef()
	{
		return this->mCurrentProcess;
	}
} // namespace Kernel

// last rev 05-03-24
