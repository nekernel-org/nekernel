/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessTeam.cc
/// @brief Process teams implementation.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.h>

namespace Kernel {
UserProcessTeam::UserProcessTeam() {
  for (SizeT i = 0U; i < this->mProcessList.Count(); ++i) {
    this->mProcessList[i]            = USER_PROCESS();
    this->mProcessList[i].PTime      = 0;
    this->mProcessList[i].RTime      = 0;
    this->mProcessList[i].Status     = ProcessStatusKind::kKilled;
    this->mProcessList[i].ParentTeam = this;
  }

  this->mProcessCount = 0UL;
}

/***********************************************************************************/
/// @brief Process list array getter.
/// @return The list of process to schedule.
/***********************************************************************************/

Array<USER_PROCESS, kSchedProcessLimitPerTeam>& UserProcessTeam::AsArray() {
  return this->mProcessList;
}

/***********************************************************************************/
/// @brief Get team ID.
/// @return The team's ID.
/***********************************************************************************/

ProcessID& UserProcessTeam::Id() noexcept {
  return this->mTeamId;
}

/***********************************************************************************/
/// @brief Get current process getter as Ref.
/// @return The current process header.
/***********************************************************************************/

Ref<USER_PROCESS>& UserProcessTeam::AsRef() {
  return this->mCurrentProcess;
}
}  // namespace Kernel

// last rev 05-03-24
