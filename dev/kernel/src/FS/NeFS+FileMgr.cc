/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef __NE_MINIMAL_OS__
#ifdef __FSKIT_INCLUDES_NEFS__

#include <KernelKit/FileMgr.h>
#include <KernelKit/HeapMgr.h>

/// @brief NeFS File System Manager.
/// BUGS: 0

namespace Kernel {
/// @brief C++ constructor
NeFileSystemMgr::NeFileSystemMgr() {
    mParser = new NeFileSystemParser();
    MUST_PASS(mParser);

    kout << "We are done allocating NeFileSystemParser...\n";
}

NeFileSystemMgr::~NeFileSystemMgr() {
    if (mParser) {
        kout << "Destroying NeFileSystemParser...\n";
        delete mParser;
        mParser = nullptr;
    }
}

/// @brief Removes a node from the filesystem.
/// @param path The filename
/// @return If it was deleted or not.
bool NeFileSystemMgr::Remove(_Input const Char* path) {
    if (path == nullptr || *path == 0) {
        kout << "NeFS: Remove called with null or empty path\n";
        return false;
    }
    return mParser->RemoveCatalog(path);
}

/// @brief Creates a node with the specified.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::Create(_Input const Char* path) {
    if (!path || *path == 0) {
        kout << "NeFS: Create called with null or empty path\n";
        return nullptr;
    }
    return rtl_node_cast(mParser->CreateCatalog(path));
}

/// @brief Creates a node which is a directory.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::CreateDirectory(const Char* path) {
    if (!path || *path == 0) {
        kout << "NeFS: CreateDirectory called with null or empty path\n";
        return nullptr;
    }
    return rtl_node_cast(mParser->CreateCatalog(path, 0, kNeFSCatalogKindDir));
}

/// @brief Creates a node which is an alias.
/// @param path The filename path.
/// @return The Node pointer.
NodePtr NeFileSystemMgr::CreateAlias(const Char* path) {
    if (!path || *path == 0) {
        kout << "NeFS: CreateAlias called with null or empty path\n";
        return nullptr;
    }
    return rtl_node_cast(mParser->CreateCatalog(path, 0, kNeFSCatalogKindAlias));
}

NodePtr NeFileSystemMgr::CreateSwapFile(const Char* path) {
    if (!path || *path == 0) {
        kout << "NeFS: CreateSwapFile called with null or empty path\n";
        return nullptr;
    }
    return rtl_node_cast(mParser->CreateCatalog(path, 0, kNeFSCatalogKindPage));
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
    if (!path || *path == 0) {
        kout << "NeFS: Open called with null or empty path\n";
        return nullptr;
    }
    if (!r || *r == 0) {
        kout << "NeFS: Open called with null or empty mode string\n";
        return nullptr;
    }
    auto catalog = mParser->GetCatalog(path);
    if (!catalog) {
        kout << "NeFS: Open could not find catalog for path\n";
        return nullptr;
    }
    return rtl_node_cast(catalog);
}

Void NeFileSystemMgr::Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
                            _Input SizeT size) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Write called with invalid node pointer\n";
        return;
    }
    if (!data) {
        kout << "NeFS: Write called with null data pointer\n";
        return;
    }
    if (!size || size > kNeFSForkSize) {
        kout << "NeFS: Write called with invalid size: " << size << "\n";
        return;
    }
    constexpr auto kDataForkName = kNeFSDataFork;
    this->Write(kDataForkName, node, data, flags, size);
}

_Output VoidPtr NeFileSystemMgr::Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT size) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Read called with invalid node pointer\n";
        return nullptr;
    }
    if (!size || size > kNeFSForkSize) {
        kout << "NeFS: Read called with invalid size: " << size << "\n";
        return nullptr;
    }
    constexpr auto kDataForkName = kNeFSDataFork;
    return this->Read(kDataForkName, node, flags, size);
}

Void NeFileSystemMgr::Write(_Input const Char* name, _Input NodePtr node, _Input VoidPtr data,
                            _Input Int32 flags, _Input SizeT size) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Write(fork) called with invalid node pointer\n";
        return;
    }
    if (!name || *name == 0) {
        kout << "NeFS: Write(fork) called with null or empty fork name\n";
        return;
    }
    if (!data) {
        kout << "NeFS: Write(fork) called with null data pointer\n";
        return;
    }
    if (!size || size > kNeFSForkSize) {
        kout << "NeFS: Write(fork) called with invalid size: " << size << "\n";
        return;
    }
    NE_UNUSED(flags);
    auto cat = reinterpret_cast<NEFS_CATALOG_STRUCT*>(node);
    if (cat->Kind == kNeFSCatalogKindFile) {
        mParser->WriteCatalog(cat->Name,
                              (flags & kFileFlagRsrc ? true : false),
                              data,
                              size,
                              name);
    }
}

_Output VoidPtr NeFileSystemMgr::Read(_Input const Char* name, _Input NodePtr node,
                                      _Input Int32 flags, _Input SizeT sz) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Read(fork) called with invalid node pointer\n";
        return nullptr;
    }
    if (!name || *name == 0) {
        kout << "NeFS: Read(fork) called with null or empty fork name\n";
        return nullptr;
    }
    if (!sz || sz > kNeFSForkSize) {
        kout << "NeFS: Read(fork) called with invalid size: " << sz << "\n";
        return nullptr;
    }
    NE_UNUSED(flags);
    auto cat = reinterpret_cast<NEFS_CATALOG_STRUCT*>(node);
    if (cat->Kind == kNeFSCatalogKindFile) {
        return mParser->ReadCatalog(cat,
                                    (flags & kFileFlagRsrc ? true : false),
                                    sz,
                                    name);
    }
    return nullptr;
}

_Output Bool NeFileSystemMgr::Seek(NodePtr node, SizeT off) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Seek called with invalid node pointer\n";
        return false;
    }
    // Allow off == 0
    return mParser->Seek(reinterpret_cast<NEFS_CATALOG_STRUCT*>(node), off);
}

/// @brief Tell current offset within catalog.
/// @param node
/// @return kFileMgrNPos if invalid, else current offset.
_Output SizeT NeFileSystemMgr::Tell(NodePtr node) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Tell called with invalid node pointer\n";
        return kFileMgrNPos;
    }
    return mParser->Tell(reinterpret_cast<NEFS_CATALOG_STRUCT*>(node));
}

/// @brief Rewinds the catalog 
/// @param node
/// @return False if invalid, nah? calls Seek(node, 0).
_Output Bool NeFileSystemMgr::Rewind(NodePtr node) {
    if (!is_valid_nefs_catalog(node)) {
        kout << "NeFS: Rewind called with invalid node pointer\n";
        return false;
    }
    return this->Seek(node, 0);
}

_Output NeFileSystemParser* NeFileSystemMgr::GetParser() noexcept {
    return mParser;
}

static inline bool is_valid_nefs_catalog(NodePtr node) {
    if (!node) return false;
    auto cat = reinterpret_cast<NEFS_CATALOG_STRUCT*>(node);
    switch (cat->Kind) {
        case kNeFSCatalogKindFile:
        case kNeFSCatalogKindDir:
        case kNeFSCatalogKindAlias:
        case kNeFSCatalogKindPage:
            break;
        default:
            return false;
    }
    bool null_found = false;
    for (int i = 0; i < kNeFSCatalogNameLen; ++i) {
        if (cat->Name[i] == 0) { null_found = true; break; }
    }
    if (!null_found) return false;
    return true;
}

}  // namespace Kernel

#endif  // ifdef __FSKIT_INCLUDES_NEFS__
#endif  // ifndef __NE_MINIMAL_OS__
