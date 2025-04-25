/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __NE_MINIMAL_OS__
#ifdef __FSKIT_INCLUDES_NEFS__

#include <KernelKit/FileMgr.h>
#include <KernelKit/MemoryMgr.h>

/// @brief NeFS File manager.
/// BUGS: 0

namespace Kernel {
/// @brief C++ constructor
NeFileSystemMgr::NeFileSystemMgr() {
  NeFileSystemParser* mParser = new NeFileSystemParser();
  MUST_PASS(mParser);

  kout << "We are done allocating NeFileSystemParser...\r";
}

NeFileSystemMgr::~NeFileSystemMgr() {
  if (mParser) {
    kout << "Destroying NeFileSystemParser...\r";
    mm_delete_class(&mParser);
  }
}

/// @brief Removes a node from the filesystem.
/// @param path The filename
/// @return If it was deleted or not.
bool NeFileSystemMgr::Remove(_Input const Char* path) {
  if (path == nullptr || *path == 0) return false;

  return mParser->RemoveCatalog(path);
}

/// @brief Creates a node with the specified.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::Create(_Input const Char* path) {
  return node_cast(mParser->CreateCatalog(path));
}

/// @brief Creates a node with is a directory.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::CreateDirectory(const Char* path) {
  return node_cast(mParser->CreateCatalog(path, 0, kNeFSCatalogKindDir));
}

/// @brief Creates a node with is a alias.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::CreateAlias(const Char* path) {
  return node_cast(mParser->CreateCatalog(path, 0, kNeFSCatalogKindAlias));
}

/// @brief Creates a node with is a page file.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::CreateSwapFile(const Char* path) {
  return node_cast(mParser->CreateCatalog(path, 0, kNeFSCatalogKindPage));
}

/// @brief Gets the root directory.
/// @return
const Char* NeFileSystemHelper::Root() {
  return kNeFSRoot;
}

/// @brief Gets the up-dir directory.
/// @return
const Char* NeFileSystemHelper::UpDir() {
  return kNeFSUpDir;
}

/// @brief Gets the separator character.
/// @return
Char NeFileSystemHelper::Separator() {
  return kNeFSSeparator;
}

/// @brief Gets the metafile character.
/// @return
Char NeFileSystemHelper::MetaFile() {
  return kNeFSMetaFilePrefix;
}

/// @brief Opens a new file.
/// @param path
/// @param r
/// @return
_Output NodePtr NeFileSystemMgr::Open(_Input const Char* path, _Input const Char* r) {
  if (!path || *path == 0) return nullptr;

  if (!r || *r == 0) return nullptr;

  auto catalog = mParser->GetCatalog(path);

  return node_cast(catalog);
}

/// @brief Writes to a catalog's fork.
/// @param node the node ptr.
/// @param data the data.
/// @param flags the size.
/// @return
Void NeFileSystemMgr::Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
                            _Input SizeT size) {
  if (!node) return;
  if (!size) return;

  constexpr auto kDataForkName = kNeFSDataFork;
  this->Write(kDataForkName, node, data, flags, size);
}

/// @brief Read from filesystem fork.
/// @param node the catalog node.
/// @param flags the flags with it.
/// @param sz the size to read.
/// @return
_Output VoidPtr NeFileSystemMgr::Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT size) {
  if (!node) return nullptr;
  if (!size) return nullptr;

  constexpr auto kDataForkName = kNeFSDataFork;
  return this->Read(kDataForkName, node, flags, size);
}

Void NeFileSystemMgr::Write(_Input const Char* name, _Input NodePtr node, _Input VoidPtr data,
                            _Input Int32 flags, _Input SizeT size) {
  if (!size || size > kNeFSForkSize) return;

  if (!data) return;

  NE_UNUSED(flags);

  if ((reinterpret_cast<NEFS_CATALOG_STRUCT*>(node))->Kind == kNeFSCatalogKindFile)
    mParser->WriteCatalog(reinterpret_cast<NEFS_CATALOG_STRUCT*>(node)->Name,
                          (flags & kFileFlagRsrc ? true : false), data, size, name);
}

_Output VoidPtr NeFileSystemMgr::Read(_Input const Char* name, _Input NodePtr node,
                                      _Input Int32 flags, _Input SizeT sz) {
  if (sz > kNeFSForkSize) return nullptr;

  if (!sz) return nullptr;

  NE_UNUSED(flags);

  if ((reinterpret_cast<NEFS_CATALOG_STRUCT*>(node))->Kind == kNeFSCatalogKindFile)
    return mParser->ReadCatalog(reinterpret_cast<NEFS_CATALOG_STRUCT*>(node),
                                (flags & kFileFlagRsrc ? true : false), sz, name);

  return nullptr;
}

/// @brief Seek from Catalog.
/// @param node
/// @param off
/// @retval true always returns false, this is unimplemented.
/// @retval false always returns this, it is unimplemented.

_Output Bool NeFileSystemMgr::Seek(NodePtr node, SizeT off) {
  if (!node || off == 0) return false;

  return mParser->Seek(reinterpret_cast<NEFS_CATALOG_STRUCT*>(node), off);
}

/// @brief Tell where the catalog is.
/// @param node
/// @retval true always returns false, this is unimplemented.
/// @retval false always returns this, it is unimplemented.

_Output SizeT NeFileSystemMgr::Tell(NodePtr node) {
  if (!node) return kNPos;

  return mParser->Tell(reinterpret_cast<NEFS_CATALOG_STRUCT*>(node));
}

/// @brief Rewinds the catalog.
/// @param node
/// @retval true always returns false, this is unimplemented.
/// @retval false always returns this, it is unimplemented.

_Output Bool NeFileSystemMgr::Rewind(NodePtr node) {
  if (!node) return false;

  return this->Seek(node, 0);
}

/// @brief Returns the filesystem parser.
/// @return the Filesystem parser class.
_Output NeFileSystemParser* NeFileSystemMgr::GetParser() noexcept {
  return mParser;
}
}  // namespace Kernel

#endif  // ifdef __FSKIT_INCLUDES_NEFS__
#endif  // ifndef __NE_MINIMAL_OS__
