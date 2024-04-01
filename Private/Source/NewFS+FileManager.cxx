/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/FileManager.hpp>

#ifdef __FSKIT_NEWFS__

/// @brief NewFS File manager.
/// BUGS: 0

namespace NewOS {
/// @brief C++ constructor
NewFilesystemManager::NewFilesystemManager() = default;

NewFilesystemManager::~NewFilesystemManager() = default;

/// @brief Removes a node from the filesystem.
/// @param fileName The filename
/// @return If it was deleted or not.
bool NewFilesystemManager::Remove(const char* fileName) {
  if (fileName == nullptr || *fileName == 0) return false;

  if (auto catalog = fImpl->GetCatalog(fileName); catalog)
    return fImpl->RemoveCatalog(catalog);

  return false;
}

/// @brief Creates a node with the specified.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NewFilesystemManager::Create(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindFile));
}

/// @brief Creates a node with is a directory.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NewFilesystemManager::CreateDirectory(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindDir));
}

/// @brief Creates a node with is a alias.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NewFilesystemManager::CreateAlias(const char* path) {
  return node_cast(fImpl->CreateCatalog(path, 0, kNewFSCatalogKindAlias));
}

/// @brief Gets the root directory.
/// @return 
const char* NewFilesystemHelper::Root() { return kNewFSRoot; }

/// @brief Gets the up-dir directory.
/// @return 
const char* NewFilesystemHelper::UpDir() { return kNewFSUpDir; }

/// @brief Gets the separator character.
/// @return 
const char NewFilesystemHelper::Separator() { return kNewFSSeparator; }
}  // namespace NewOS

#endif  // ifdef __FSKIT_NEWFS__
