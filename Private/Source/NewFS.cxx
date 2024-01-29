/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FSKit/NewFS.hxx>
#include <KernelKit/FileManager.hpp>

namespace hCore {
NewFilesystemManager::NewFilesystemManager() = default;

NewFilesystemManager::~NewFilesystemManager() = default;

/**
 * Unallocates a file from disk.
 * @param node_name it's path.
 * @return operation status boolean.
 */
bool NewFilesystemManager::Remove(const char* node_name) {
  if (node_name == nullptr || *node_name == 0) return false;

  if (auto catalog = fIO->GetCatalog(node_name); catalog)
    return fIO->RemoveCatalog(*catalog);

  return false;
}

NodePtr NewFilesystemManager::Create(const char* path) {
  return node_cast(fIO->CreateCatalog(path, 0, kCatalogKindFile));
}

NodePtr NewFilesystemManager::CreateDirectory(const char* path) {
  return node_cast(fIO->CreateCatalog(path, 0, kCatalogKindDir));
}

NodePtr NewFilesystemManager::CreateAlias(const char* path) {
  return node_cast(fIO->CreateCatalog(path, 0, kCatalogKindAlias));
}
}  // namespace hCore
