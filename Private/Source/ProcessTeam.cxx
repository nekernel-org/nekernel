/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief Process Team API.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hpp>

namespace HCore {
MutableArray<Ref<Process>>& ProcessTeam::AsArray() { return mProcessList; }
Ref<Process>& ProcessTeam::AsRef() { return mCurrentProcess; }
} // namespace HCore

// last rev 05-03-24
