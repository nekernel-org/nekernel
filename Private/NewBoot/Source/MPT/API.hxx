/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

namespace mpt
{
    /// initializes the Master Partition Table and the Files32 filesystem.
    /// \return status, assert(fail) is also triggered, use filesystem_hook_error if you want to catch it.
    bool init_mpt() noexcept;
}