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
    hCore::SizeT DiskId{ 0 };
    hCore::VoidPtr DiskSpace{ nullptr };
    hCore::SizeT DiskSize{ 0 };
    hCore::Int32 DiskError{ 0 };

    FileType* Read(const char* path) { return _Manager::Read(path); }
    FileType* Write(FileType* path) { return _Manager::Write(path); }

};