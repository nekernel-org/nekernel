/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/FileManager.hpp>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Utils.hpp>

//! @brief File manager for hCore.

namespace hCore
{
    static IFilesystemManager* kMounted = nullptr;
    
    /// @brief FilesystemManager getter.
    /// @return The mounted filesystem.
    IFilesystemManager* IFilesystemManager::GetMounted() { return kMounted; }

    IFilesystemManager* IFilesystemManager::Unmount()
    {
        if (kMounted)
        {
            auto mount = kMounted;
            kMounted = nullptr;

            return mount;
        }

        return nullptr;
    }

    bool IFilesystemManager::Mount(IFilesystemManager* pMount)
    {
        if (pMount)
        {
            kMounted = pMount;
            return true;
        }
        
        return false;
    }
}
