/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

// @brief this file purpose is to read/write files.

#include <NewKit/Defines.hpp>

/// \brief File buffer class
/// \tparam _Manager The disk manager
template <typename _Manager>
class FileType
{
public:
    hCore::SizeT DiskId{ 0 }; // identification number of the drive.
    hCore::VoidPtr DiskSpace{ nullptr }; // the pointer containing the requested disk data.
    hCore::SizeT DiskSize{ 0 }; // it's size
    hCore::Int32 DiskError{ 0 }; // if it's going well.

    FileType* Read(const char* path) { return _Manager::Read(path); }
    FileType* Write(FileType* path) { return _Manager::Write(path); }

    // little sanity test
    operator bool()
    {
        return DiskError != 0 && DiskSize > 0;
    }

};