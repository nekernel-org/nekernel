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

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Stream.hpp>

/// Main filesystem abstraction manager.

#define kBootFolder "/Boot"
#define kBinFolder "/Programs"
#define kShLibsFolder "/Library"

#define kSectorSz 512

/// refer to first enum.
#define kFileOpsCount 4

namespace HCore {
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
class IFilesystemManager {
 public:
  IFilesystemManager() = default;
  virtual ~IFilesystemManager() = default;

 public:
  HCORE_COPY_DEFAULT(IFilesystemManager);

 public:
  static bool Mount(IFilesystemManager *pMount);
  static IFilesystemManager *Unmount();
  static IFilesystemManager *GetMounted();

 public:
  virtual NodePtr Create(const char *path) = 0;
  virtual NodePtr CreateAlias(const char *path) = 0;
  virtual NodePtr CreateDirectory(const char *path) = 0;

 public:
  virtual bool Remove(const char *path) = 0;

 public:
  virtual NodePtr Open(const char *path, const char *r) = 0;

 public:
  virtual void Write(NodePtr node, VoidPtr data, Int32 flags) = 0;
  virtual VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) = 0;

 public:
  virtual bool Seek(NodePtr node, SizeT off) = 0;

 public:
  virtual SizeT Tell(NodePtr node) = 0;
  virtual bool Rewind(NodePtr node) = 0;
};

/** @brief invalid position. (n-pos) */
#define kNPos (SizeT)(-1);

#ifdef __FSKIT_NEWFS__
/**
 * @brief Based of IFilesystemManager, takes care of managing NewFS disks.
 */
class NewFilesystemManager final : public IFilesystemManager {
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
  bool Remove(const char *node) override;

 public:
  NodePtr Open(const char *path, const char *r) override {
    if (!path || *path == 0) return nullptr;

    if (!r || *r == 0) return nullptr;

    return this->Open(path, r);
  }

 public:
  Void Write(NodePtr node, VoidPtr data, Int32 flags) override {
    this->Write(node, data, flags);
  }

  VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) override {
    return this->Read(node, flags, sz);
  }

 public:
  bool Seek(NodePtr node, SizeT off) override {
    if (!node || off == 0) return false;

    return this->Seek(node, off);
  }

 public:
  SizeT Tell(NodePtr node) override {
    if (!node) return kNPos;

    return this->Tell(node);
  }

  bool Rewind(NodePtr node) override {
    if (!node) return false;

    return this->Seek(node, 0);
  }

 public:
  NewFSImplementation *fImpl{nullptr};
};

#endif  // ifdef __FSKIT_NEWFS__

/**
 * Usable FileStream
 * @tparam Encoding file encoding (char, wchar_t...)
 * @tparam FSClass Filesystem contract who takes care of it.
 */
template <typename Encoding = char, typename FSClass = IFilesystemManager>
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

  void Write(SizeT offset, voidPtr data, SizeT sz) {
    auto man = FSClass::GetMounted();

    if (man) {
      man->Seek(fFile, offset);
      man->Write(fFile, data, sz, kFileReadChunk);
    }
  }

 public:
  char *MIME() noexcept { return const_cast<char *>(fMime); }

 private:
  NodePtr fFile;
  const Char *fMime{"application-type/*"};
};

#define kRestrictRW "r+"
#define kRestrictRWB "r+b"
#define kRestrictR "r"
#define kRestrictRB "rb"

using FileStreamUTF8 = FileStream<char>;
using FileStreamUTF16 = FileStream<wchar_t>;

typedef UInt64 CursorType;

template <typename Encoding, typename Class>
FileStream<Encoding, Class>::FileStream(const Encoding *path,
                                        const Encoding *restrict_type)
    : fFile(Class::GetMounted()->Open(path, restrict_type)) {}

template <typename Encoding, typename Class>
FileStream<Encoding, Class>::~FileStream() = default;
}  // namespace HCore

#define node_cast(PTR) reinterpret_cast<HCore::NodePtr>(PTR)
