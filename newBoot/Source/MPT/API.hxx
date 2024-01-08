/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

namespace mpt
{
/// initializes the Master Partition Table Files32 filesystem.
/// \return status, assert(fail) is also triggered, use filesystem_hook_error if you want to catch it.
bool filesystem_init(void);
}