/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

extern "C" HCore::UIntPtr* __EXEC_IVT;

extern void rt_wait_for_io(void);

namespace HCore {
bool ke_init_hal() { return true; }
}  // namespace HCore
