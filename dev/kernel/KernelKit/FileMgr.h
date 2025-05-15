/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss Labs, all rights reserved.

  File: FileMgr.h
  Purpose: Kernel file manager.

------------------------------------------- */

/* -------------------------------------------

 Revision History:

   31/01/24: Update documentation (amlel)
   05/07/24: NeFS support, and fork support, updated constants and specs
    as well.
  18/01/25: Patches to FileStream class.

 ------------------------------------------- */

#ifndef INC_FILEMGR_H
#define INC_FILEMGR_H

//! Include filesystems that neoskrnl supports.
#include <FSKit/Ext2.h>
#include <FSKit/HeFS.h>
#include <FSKit/NeFS.h>

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/KPC.h>
#include <KernelKit/MemoryMgr.h>
#include <NewKit/ErrorOr.h>
#include <NewKit/Ref.h>
#include <NewKit/Stream.h>
#include <hint/CompilerHint.h>

/// @brief Filesystem manager, abstraction over mounted filesystem.
/// Works like an VFS (Virtual File System) or IFS subsystem on NT/OS 2.

#define kRestrictR "r"
#define kRestrictRB "rb"
#define kRestrictW "w"
#define kRestrictWR "rw"
#define kRestrictWRB "rwb"

#define kRestrictMax (5U)

#define rtl_node_cast(PTR) reinterpret_cast<Kernel::NodePtr>(PTR)

/**
  @note Refer to first enum.
*/
#define kFileOpsCount (4U)
#define kFileMimeGeneric "ne-application-kind/all"

/** @brief invalid position. (n-pos) */
#define kFileMgrNPos (~0UL)

namespace Kernel {
enum {
  kFileIOInvalid  = 0,
  kFileWriteAll   = 100,
  kFileReadAll    = 101,
  kFileReadChunk  = 102,
  kFileWriteChunk = 103,
  kFileIOCnt      = (kFileWriteChunk - kFileWriteAll) + 1,
  // File flags (HFS+, NeFS specific)
  kFileFlagRsrc = 104,
  kFileFlagData = 105,
};

typedef VoidPtr NodePtr;

/**
@brief Filesystem Mgr Interface class
@brief Used to provide common I/O for a specific filesystem.
*/
class IFilesystemMgr {
 public:
  explicit IFilesystemMgr() = default;
  virtual ~IFilesystemMgr() = default;

 public:
  NE_COPY_DEFAULT(IFilesystemMgr)

 public:
  /// @brief Mounts a new filesystem into an active state.
  /// @param interface the filesystem interface
  /// @return
  static bool Mount(IFilesystemMgr* interface);

  /// @brief Unmounts the active filesystem
  /// @return
  static IFilesystemMgr* Unmount();

  /// @brief Getter, gets the active filesystem.
  /// @return
  static IFilesystemMgr* GetMounted();

 public:
  virtual NodePtr Create(_Input const Char* path)          = 0;
  virtual NodePtr CreateAlias(_Input const Char* path)     = 0;
  virtual NodePtr CreateDirectory(_Input const Char* path) = 0;
  virtual NodePtr CreateSwapFile(const Char* path)         = 0;

 public:
  virtual bool Remove(_Input const Char* path) = 0;

 public:
  virtual NodePtr Open(_Input const Char* path, _Input const Char* r) = 0;

 public:
  virtual Void Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
                     _Input SizeT size) = 0;

  virtual _Output VoidPtr Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT sz) = 0;

  virtual Void Write(_Input const Char* name, _Input NodePtr node, _Input VoidPtr data,
                     _Input Int32 flags, _Input SizeT size) = 0;

  virtual _Output VoidPtr Read(_Input const Char* name, _Input NodePtr node, _Input Int32 flags,
                               _Input SizeT sz) = 0;

 public:
  virtual bool Seek(_Input NodePtr node, _Input SizeT off) = 0;

 public:
  virtual SizeT Tell(_Input NodePtr node)   = 0;
  virtual bool  Rewind(_Input NodePtr node) = 0;
};

#ifdef __FSKIT_INCLUDES_NEFS__
/**
 * @brief Based of IFilesystemMgr, takes care of managing NeFS
 * disks.
 */
class NeFileSystemMgr final : public IFilesystemMgr {
 public:
  explicit NeFileSystemMgr();
  ~NeFileSystemMgr() override;

 public:
  NE_COPY_DEFAULT(NeFileSystemMgr)

 public:
  NodePtr Create(const Char* path) override;
  NodePtr CreateAlias(const Char* path) override;
  NodePtr CreateDirectory(const Char* path) override;
  NodePtr CreateSwapFile(const Char* path) override;

 public:
  bool    Remove(_Input const Char* path) override;
  NodePtr Open(_Input const Char* path, _Input const Char* r) override;
  Void    Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
                _Input SizeT sz) override;
  VoidPtr Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT sz) override;
  bool    Seek(_Input NodePtr node, _Input SizeT off) override;
  SizeT   Tell(_Input NodePtr node) override;
  bool    Rewind(_Input NodePtr node) override;

  Void Write(_Input const Char* name, _Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
             _Input SizeT size) override;

  _Output VoidPtr Read(_Input const Char* name, _Input NodePtr node, _Input Int32 flags,
                       _Input SizeT sz) override;

 public:
  /// @brief Get NeFS parser class.
  /// @return The filesystem parser class.
  NeFileSystemParser* GetParser() noexcept;

 private:
  NeFileSystemParser* mParser{nullptr};
};

#endif  // ifdef __FSKIT_INCLUDES_NEFS__

#ifdef __FSKIT_INCLUDES_HEFS__
/**
 * @brief Based of IFilesystemMgr, takes care of managing NeFS
 * disks.
 */
class HeFileSystemMgr final : public IFilesystemMgr {
 public:
  explicit HeFileSystemMgr();
  ~HeFileSystemMgr() override;

 public:
  NE_COPY_DEFAULT(HeFileSystemMgr)

 public:
  NodePtr Create(const Char* path) override;
  NodePtr CreateAlias(const Char* path) override;
  NodePtr CreateDirectory(const Char* path) override;
  NodePtr CreateSwapFile(const Char* path) override;

 public:
  bool    Remove(_Input const Char* path) override;
  NodePtr Open(_Input const Char* path, _Input const Char* r) override;
  Void    Write(_Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
                _Input SizeT sz) override;
  VoidPtr Read(_Input NodePtr node, _Input Int32 flags, _Input SizeT sz) override;
  bool    Seek(_Input NodePtr node, _Input SizeT off) override;
  SizeT   Tell(_Input NodePtr node) override;
  bool    Rewind(_Input NodePtr node) override;

  Void Write(_Input const Char* name, _Input NodePtr node, _Input VoidPtr data, _Input Int32 flags,
             _Input SizeT size) override;

  _Output VoidPtr Read(_Input const Char* name, _Input NodePtr node, _Input Int32 flags,
                       _Input SizeT sz) override;

 public:
  /// @brief Get NeFS parser class.
  /// @return The filesystem parser class.
  HeFileSystemParser* GetParser() noexcept;

 private:
  HeFileSystemParser* mParser{nullptr};
};

#endif  // ifdef __FSKIT_INCLUDES_HEFS__

/**
 * FileStream class.
 * @tparam Encoding file encoding (char, wchar_t...)
 * @tparam FSClass Filesystem contract who takes care of it.
 */
template <typename Encoding = Char, typename FSClass = IFilesystemMgr>
class FileStream final {
 public:
  explicit FileStream(const Encoding* path, const Encoding* restrict_type);
  ~FileStream();

 public:
  FileStream& operator=(const FileStream&);
  FileStream(const FileStream&);

 public:
  ErrorOr<Int64> Write(SizeT offset, const VoidPtr data, SizeT len) noexcept {
    if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
        this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
        this->fFileRestrict != kFileMgrRestrictWrite &&
        this->fFileRestrict != kFileMgrRestrictWriteBinary)
      return ErrorOr<Int64>(kErrorInvalidData);

    if (data == nullptr) return ErrorOr<Int64>(kErrorInvalidData);

    auto man = FSClass::GetMounted();

    if (man) {
      man->Write(offset, fFile, data, len);
      return ErrorOr<Int64>(0);
    }

    return ErrorOr<Int64>(kErrorInvalidData);
  }

  ErrorOr<Int64> Write(const Char* name, const VoidPtr data, SizeT len) noexcept {
    if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
        this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
        this->fFileRestrict != kFileMgrRestrictWrite &&
        this->fFileRestrict != kFileMgrRestrictWriteBinary)
      return ErrorOr<Int64>(kErrorInvalidData);

    if (data == nullptr) return ErrorOr<Int64>(kErrorInvalidData);

    auto man = FSClass::GetMounted();

    if (man) {
      man->Write(name, fFile, data, 0, len);
      return ErrorOr<Int64>(0);
    }

    return ErrorOr<Int64>(kErrorInvalidData);
  }

  VoidPtr Read(const Char* name, SizeT sz) noexcept {
    if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
        this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
        this->fFileRestrict != kFileMgrRestrictRead &&
        this->fFileRestrict != kFileMgrRestrictReadBinary)
      return nullptr;

    NE_UNUSED(sz);

    auto man = FSClass::GetMounted();

    if (man) {
      VoidPtr ret = man->Read(name, fFile, kFileReadAll, 0);
      return ret;
    }

    return nullptr;
  }

  VoidPtr Read(SizeT offset, SizeT sz) {
    if (this->fFileRestrict != kFileMgrRestrictReadWrite &&
        this->fFileRestrict != kFileMgrRestrictReadWriteBinary &&
        this->fFileRestrict != kFileMgrRestrictRead &&
        this->fFileRestrict != kFileMgrRestrictReadBinary)
      return nullptr;

    auto man = FSClass::GetMounted();

    if (man) {
      man->Seek(fFile, offset);
      auto ret = man->Read(fFile, kFileReadChunk, sz);

      return ret;
    }

    return nullptr;
  }

 public:
  /// @brief Leak node pointer.
  /// @return The node pointer.
  NodePtr Leak() { return fFile; }

  /// @brief Leak MIME.
  /// @return The MIME.
  Char* MIME() noexcept { return const_cast<Char*>(fMime); }

  enum {
    kFileMgrRestrictRead,
    kFileMgrRestrictReadBinary,
    kFileMgrRestrictWrite,
    kFileMgrRestrictWriteBinary,
    kFileMgrRestrictReadWrite,
    kFileMgrRestrictReadWriteBinary,
  };

 private:
  NodePtr     fFile{nullptr};
  Int32       fFileRestrict{kFileMgrRestrictReadBinary};
  const Char* fMime{kFileMimeGeneric};
};

using FileStreamUTF8  = FileStream<Char>;
using FileStreamUTF16 = FileStream<WideChar>;

typedef UInt64 CursorType;

inline static const auto kRestrictStrLen = 8U;

/// @brief restrict information about the file descriptor.
struct FileRestrictKind final {
  Char  fRestrict[kRestrictStrLen];
  Int32 fMappedTo;
};

/// @brief constructor
template <typename Encoding, typename Class>
inline FileStream<Encoding, Class>::FileStream(const Encoding* path, const Encoding* restrict_type)
    : fFile(Class::GetMounted()->Open(path, restrict_type)) {
  SizeT                  kRestrictCount  = kRestrictMax;
  const FileRestrictKind kRestrictList[] = {{
                                                .fRestrict = kRestrictR,
                                                .fMappedTo = kFileMgrRestrictRead,
                                            },
                                            {
                                                .fRestrict = kRestrictRB,
                                                .fMappedTo = kFileMgrRestrictReadBinary,
                                            },
                                            {
                                                .fRestrict = kRestrictWRB,
                                                .fMappedTo = kFileMgrRestrictReadWriteBinary,
                                            },
                                            {
                                                .fRestrict = kRestrictW,
                                                .fMappedTo = kFileMgrRestrictWrite,
                                            },
                                            {
                                                .fRestrict = kRestrictWR,
                                                .fMappedTo = kFileMgrRestrictReadWrite,
                                            }};

  for (SizeT index = 0; index < kRestrictCount; ++index) {
    if (rt_string_cmp(restrict_type, kRestrictList[index].fRestrict,
                      rt_string_len(kRestrictList[index].fRestrict)) == 0) {
      fFileRestrict = kRestrictList[index].fMappedTo;
      break;
    }
  }

  kout << "FileMgr: New file at: " << path << ".\r";
}

/// @brief destructor of the file stream.
template <typename Encoding, typename Class>
inline FileStream<Encoding, Class>::~FileStream() {
  mm_delete_ptr(fFile);
}
}  // namespace Kernel

#endif  // ifndef INC_FILEMGR_H
