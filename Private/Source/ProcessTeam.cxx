/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessTeam.cxx
/// @brief ProcessHeader Team API.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hpp>

namespace HCore {
/// @brief Process list array getter.
/// @return 
MutableArray<Ref<ProcessHeader>>& ProcessTeam::AsArray() { return mProcessList; }

/// @brief Current process getter.
/// @return 
Ref<ProcessHeader>& ProcessTeam::AsRef() { return mCurrentProcess; }
} // namespace HCore

// last rev 05-03-24
