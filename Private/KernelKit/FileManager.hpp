/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/* -------------------------------------------

 Revision History:

     31/01/24: Update documentation (amlel)

 ------------------------------------------- */

#pragma once

#ifdef __FSKIT_NEWFS__
#include <FSKit/NewFS.hxx>
#endif  // __FSKIT_NEWFS__

#ifdef __FSKIT_HCFS__
#include <FSKit/HCFS.hxx>
#endif  // __FSKIT_HCFS__

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Stream.hpp>

/// @brief Filesystem abstraction manager.
/// Works like the VFS or IFS.

#define kBootFolder "/Boot"
#define kBinFolder "/Applications"
#define kShLibsFolder "/Library"
#define kMountFolder "/Mount"

/// refer to first enum.
#define kFileOpsCount 4
#define kFileMimeGeneric "application-type/*"

namespace NewOS {
enum {
  kFileWriteAll = 100,
  kFileReadAll = 101,
  kFileReadChunk = 102,
  kFileWriteChunk = 103,
  kFileIOCnt = (kFileWriteChunk - kFileWriteAll) + 1,
};

typedef VoidPtr NodePtr;

/**
    @brief Filesystem Manager Interface class
    @brief Used to provide common I/O for a specific filesystem.
*/
class FilesystemManagerInterface {
 public:
  FilesystemManagerInterface() = default;
  virtual ~FilesystemManagerInterface() = default;

 public:
  HCORE_COPY_DEFAULT(FilesystemManagerInterface);

 public:
  /// @brief Mounts a new filesystem into an active state.
  /// @param interface the filesystem interface
  /// @return
  static bool Mount(FilesystemManagerInterface *interface);

  /// @brief Unmounts the active filesystem
  /// @return
  static FilesystemManagerInterface *Unmount();

  /// @brief Getter, gets the active filesystem.
  /// @return
  static FilesystemManagerInterface *GetMounted();

 public:
  virtual NodePtr Create(_Input const char *path) = 0;
  virtual NodePtr CreateAlias(_Input const char *path) = 0;
  virtual NodePtr CreateDirectory(_Input const char *path) = 0;

 public:
  virtual bool Remove(_Input const char *path) = 0;

 public:
  virtual NodePtr Open(_Input const char *path, _Input const char *r) = 0;

 public:
  virtual Void Write(_Input NodePtr node, _Input VoidPtr data,
                     _Input Int32 flags) = 0;
  virtual _Output VoidPtr Read(_Input NodePtr node, _Input Int32 flags,
                               _Input SizeT sz) = 0;

 public:
  virtual bool Seek(_Input NodePtr node, _Input SizeT off) = 0;

 public:
  virtual SizeT Tell(_Input NodePtr node) = 0;
  virtual bool Rewind(_Input NodePtr node) = 0;
};

/** @brief invalid position. (n-pos) */
#define kNPos (SizeT)(-1);

#ifdef __FSKIT_NEWFS__
/**
 * @brief Based of FilesystemManagerInterface, takes care of managing NewFS
 * disks.
 */
class NewFilesystemManager final : public FilesystemManagerInterface {
 public:
  explicit NewFilesystemManager();
  ~NewFilesystemManager() override;

 public:
  HCORE_COPY_DEFAULT(NewFilesystemManager);

 public:
  NodePtr Create(const char *path) override;
  NodePtr CreateAlias(const char *path) override;
  NodePtr CreateDirectory(const char *path) override;

 public:
  bool Remove(const char *path) override;
  NodePtr Open(const char *path, const char *r) override;
  Void Write(NodePtr node, VoidPtr data, Int32 flags) override;
  VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) override;
  bool Seek(NodePtr node, SizeT off);
  SizeT Tell(NodePtr node) override;
  bool Rewind(NodePtr node) override;

  NewFSParser* GetImpl() noexcept;

 public:
  NewFSParser *fImpl{nullptr};
};

#endif  // ifdef __FSKIT_NEWFS__

/**
 * Usable FileStream
 * @tparam Encoding file encoding (char, wchar_t...)
 * @tparam FSClass Filesystem contract who takes care of it.
 */
template <typename Encoding = char,
          typename FSClass = FilesystemManagerInterface>
class FileStream final {
 public:
  explicit FileStream(const Encoding *path, const Encoding *restrict_type);
  ~FileStream();

 public:
  FileStream &operator=(const FileStream &);
  FileStream(const FileStream &);

 public:
  ErrorOr<Int64> WriteAll(const VoidPtr data) noexcept {
    if (data == nullptr) return ErrorOr<Int64>(H_INVALID_DATA);

    auto man = FSClass::GetMounted();

    if (man) {
      man->Write(fFile, data, kFileWriteAll);
      return ErrorOr<Int64>(0);
    }

    return ErrorOr<Int64>(H_INVALID_DATA);
  }

  VoidPtr Read() noexcept {
    auto man = FSClass::GetMounted();

    if (man) {
      VoidPtr ret = man->Read(fFile, kFileReadAll, 0);
      return ret;
    }

    return nullptr;
  }

  voidPtr Read(SizeT offset, SizeT sz) {
    auto man = FSClass::GetMounted();

    if (man) {
      man->Seek(fFile, offset);
      auto ret = man->Read(fFile, kFileReadChunk, sz);

      return ret;
    }

    return nullptr;
  }

  Void Write(SizeT offset, voidPtr data, SizeT sz) {
    auto man = FSClass::GetMounted();

    if (man) {
      man->Seek(fFile, offset);
      man->Write(fFile, data, sz, kFileReadChunk);
    }
  }

  /// @brief Leak node pointer.
  /// @return The node pointer.
  NodePtr Leak() { return fFile; }

 public:
  char *MIME() noexcept { return const_cast<char *>(fMime); }

 private:
  NodePtr fFile;
  const Char *fMime{kFileMimeGeneric};
};

#define kRestrictR "r"
#define kRestrictRB "rb"
#define kRestrictW "w"
#define kRestrictRW "rw"

using FileStreamUTF8 = FileStream<Char>;
using FileStreamUTF16 = FileStream<WideChar>;

typedef UInt64 CursorType;

template <typename Encoding, typename Class>
FileStream<Encoding, Class>::FileStream(const Encoding *path,
                                        const Encoding *restrict_type)
    : fFile(Class::GetMounted()->Open(path, restrict_type)) {}

template <typename Encoding, typename Class>
FileStream<Encoding, Class>::~FileStream() = default;
}  // namespace NewOS

#define node_cast(PTR) reinterpret_cast<NewOS::NodePtr>(PTR)
