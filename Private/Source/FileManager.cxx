/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/FileManager.hpp>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Utils.hpp>

/// BUGS: 0
//! @brief File manager for NewOS.

namespace NewOS {
static FilesystemManagerInterface* kMounted = nullptr;

/// @brief FilesystemManager getter.
/// @return The mounted filesystem.
FilesystemManagerInterface* FilesystemManagerInterface::GetMounted() {
  return kMounted;
}

/// @brief Unmount filesystem.
/// @return The unmounted filesystem.
FilesystemManagerInterface* FilesystemManagerInterface::Unmount() {
  if (kMounted) {
    auto mount = kMounted;
    kMounted = nullptr;

    return mount;
  }

  return nullptr;
}

/// @brief Mount filesystem.
/// @param mountPtr The filesystem to mount.
/// @return if it succeeded true, otherwise false.
bool FilesystemManagerInterface::Mount(FilesystemManagerInterface* mountPtr) {
  if (kMounted == nullptr) {
    kMounted = mountPtr;
    return true;
  }

  return false;
}

#ifdef __FSKIT_NEWFS__
/// @brief Opens a new file.
/// @param path
/// @param r
/// @return
NodePtr NewFilesystemManager::Open(const char* path, const char* r) {
  if (!path || *path == 0) return nullptr;

  if (!r || *r == 0) return nullptr;

  auto catalog = fImpl->GetCatalog(path);

  if (catalog->Kind != kNewFSCatalogKindFile) {
    fImpl->CloseCatalog(catalog);
    return nullptr;
  }

  return node_cast(catalog);
}

/// @brief Writes to a catalog
/// @param node
/// @param data
/// @param flags
/// @return
Void NewFilesystemManager::Write(NodePtr node, VoidPtr data,
                                       Int32 flags, SizeT size) {
  if ((reinterpret_cast<NewCatalog*>(node))->Kind == kNewFSCatalogKindFile)
    fImpl->WriteCatalog(reinterpret_cast<NewCatalog*>(node), data, size);
}

/**
 * NOTE: Write and Read are implemented using a custom NodePtr, retrieved
 * using OpenFork.
 */

/// @brief Reads from filesystem.
/// @param node
/// @param flags
/// @param sz
/// @return
VoidPtr NewFilesystemManager::Read(NodePtr node, Int32 flags, SizeT sz) {
  if ((reinterpret_cast<NewCatalog*>(node))->Kind == kNewFSCatalogKindFile)
    return fImpl->ReadCatalog(reinterpret_cast<NewCatalog*>(node)->Name, sz);

  return nullptr;
}

/// @brief Seek from Catalog.
/// @param node
/// @param off
/// @return
bool NewFilesystemManager::Seek(NodePtr node, SizeT off) {
  if (!node || off == 0) return false;

  return fImpl->Seek(reinterpret_cast<NewCatalog*>(node), off);
}

/// @brief Tell where the catalog is/
/// @param node
/// @return
SizeT NewFilesystemManager::Tell(NodePtr node) {
  if (!node) return kNPos;

  return fImpl->Tell(reinterpret_cast<NewCatalog*>(node));
}

/// @brief Rewind the catalog.
/// @param node
/// @return
bool NewFilesystemManager::Rewind(NodePtr node) {
  if (!node) return false;

  return this->Seek(node, 0);
}

/// @brief The filesystem implementation.
/// @return
NewFSParser* NewFilesystemManager::GetImpl() noexcept { return fImpl; }
#endif // __FSKIT_NEWFS__
}  // namespace NewOS
