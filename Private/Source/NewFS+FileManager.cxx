/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/FileManager.hpp>

#ifdef __FSKIT_NEWFS__

/// @brief NewFS File manager.
/// BUGS: 0

namespace NewOS {
NewFilesystemManager::NewFilesystemManager() = default;

NewFilesystemManager::~NewFilesystemManager() = default;

bool NewFilesystemManager::Remove(const char* node_name) {
  if (node_name == nullptr || *node_name == 0) return false;

  if (auto catalog = fImpl->GetCatalog(node_name); catalog)
    return fImpl->RemoveCatalog(catalog);

  return false;
}

NodePtr NewFilesystemManager::Create(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindFile));
}

NodePtr NewFilesystemManager::CreateDirectory(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindDir));
}

NodePtr NewFilesystemManager::CreateAlias(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindAlias));
}

const char* NewFilesystemHelper::Root() { return kNewFSRoot; }

const char* NewFilesystemHelper::UpDir() { return kNewFSUpDir; }

const char NewFilesystemHelper::Separator() { return kNewFSSeparator; }
}  // namespace NewOS

#endif  // ifdef __FSKIT_NEWFS__
