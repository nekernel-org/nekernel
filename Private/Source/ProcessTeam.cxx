/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief ProcessHeader Team API.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hpp>

namespace HCore {
MutableArray<Ref<ProcessHeader>>& ProcessTeam::AsArray() { return mProcessList; }
Ref<ProcessHeader>& ProcessTeam::AsRef() { return mCurrentProcess; }
} // namespace HCore

// last rev 05-03-24
