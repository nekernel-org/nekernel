/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/FileManager.hpp>

#ifdef __USE_NEWFS__

namespace HCore {
NewFilesystemManager::NewFilesystemManager() = default;

NewFilesystemManager::~NewFilesystemManager() = default;

/**
 * Unallocates a file from disk.
 * @param node_name it's path.
 * @return operation status boolean.
 */
bool NewFilesystemManager::Remove(const char* node_name) {
  if (node_name == nullptr || *node_name == 0) return false;

  if (auto catalog = fImpl->GetCatalog(node_name); catalog)
    return fImpl->RemoveCatalog(*catalog);

  return false;
}

NodePtr NewFilesystemManager::Create(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kCatalogKindFile));
}

NodePtr NewFilesystemManager::CreateDirectory(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kCatalogKindDir));
}

NodePtr NewFilesystemManager::CreateAlias(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kCatalogKindAlias));
}

const char* NewFilesystemHelper::Root() { return kFilesystemRoot; }

const char* NewFilesystemHelper::UpDir() { return kFilesystemUpDir; }

const char NewFilesystemHelper::Separator() { return kFilesystemSeparator; }
}  // namespace HCore

#endif  // ifdef __USE_NEWFS__
