/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FSKit/NewFS.hxx>
#include <KernelKit/FileManager.hpp>

namespace hCore
{	
    NewFilesystemManager::NewFilesystemManager() = default;
    
    NewFilesystemManager::~NewFilesystemManager() = default;

    bool NewFilesystemManager::Remove(const char* node)
    {
        if (node == nullptr ||
            *node == 0)
            return false;

        if (auto catalog = fIO->GetCatalog(node);
            catalog->Flags == kFlagCatalog)
            return fIO->RemoveCatalog(*catalog);
        
        return false;
    }

    NodePtr NewFilesystemManager::Create(const char* path)
    {
        return node_cast(fIO->CreateCatalog(path, 0, kCatalogKindFile));
    }

    NodePtr NewFilesystemManager::CreateDirectory(const char *path)
    {
        return node_cast(fIO->CreateCatalog(path, 0, kCatalogKindDir));
    }

    NodePtr NewFilesystemManager::CreateAlias(const char *path)
    {
        return node_cast(fIO->CreateCatalog(path, 0, kCatalogKindAlias));
    }
} // namespace hCore
