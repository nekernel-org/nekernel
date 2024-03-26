/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __FILE_API__
#define __FILE_API__

#include <System.Core/Headers/Defines.hxx>

#define kFileOpenInterface 10
#define kFileAliasInterface 11

namespace System {
class FileInterface;
class DirectoryInterface;

typedef IntPtrType FSRef;

enum {
  kErrorNoSuchPath = -44,
  kErrorNotAFile = -45,
  kErrorNotADirectory = -46,
  kErrorDirectory = -47,
  kErrorBrokenSymlink = -48,
};

/// @brief System file interface

class FileInterface final {
 public:
  explicit FileInterface(const char *path) {
    CA_MUST_PASS(path);

    mHandle = kApplicationObject->Invoke(
        kApplicationObject, kProcessCallOpenHandle, kFileOpenInterface, path);
  }

  ~FileInterface() {
    CA_MUST_PASS(kApplicationObject);

    kApplicationObject->Invoke(kApplicationObject, kProcessCallCloseHandle,
                               kFileOpenInterface, mHandle);
  }

 public:
  CA_COPY_DEFAULT(FileInterface);

 public:
  PtrVoidType Read(UIntPtrType off, SizeType sz) {
    return (PtrVoidType)kApplicationObject->Invoke(kApplicationObject, mHandle,
                                                   2, off, sz);
  }

  PtrVoidType Read(SizeType sz) {
    return (PtrVoidType)kApplicationObject->Invoke(kApplicationObject, mHandle,
                                                   3, sz);
  }

  void Write(PtrVoidType buf, UIntPtrType off, SizeType sz) {
    kApplicationObject->Invoke(kApplicationObject, mHandle, 4, buf, off, sz);
  }

  void Write(PtrVoidType buf, SizeType sz) {
    kApplicationObject->Invoke(kApplicationObject, mHandle, 5, buf, sz);
  }

  void Seek(UIntPtrType off) {
    kApplicationObject->Invoke(kApplicationObject, mHandle, 5);
  }

  void Rewind() { kApplicationObject->Invoke(kApplicationObject, mHandle, 6); }

 public:
  const char *MIME() {
    return (const char *)kApplicationObject->Invoke(kApplicationObject, mHandle,
                                                    7);
  }

  void MIME(const char *mime) {
    kApplicationObject->Invoke(kApplicationObject, mHandle, 8, mime);
  }

 private:
  FSRef mHandle;
};

typedef FileInterface *FilePtr;

/// @brief file exception
/// Throws when the file isn't found or invalid.
class FileException : public SystemException {
 public:
  explicit FileException() = default;
  virtual ~FileException() = default;

 public:
  CA_COPY_DEFAULT(FileException);

 public:
  const char *Name() override { return "FileException"; }
  const char *Reason() override { return mReason; }

 private:
  const char *mReason{"System.Core: FileException: Catastrophic failure!"};
};

inline IntPtrType FSMakeAlias(const char *from, const char *outputWhere) {
  CA_MUST_PASS(from);
  CA_MUST_PASS(outputWhere);

  return kApplicationObject->Invoke(kApplicationObject, kProcessCallOpenHandle,
                                    kFileAliasInterface, from);
}
}  // namespace System

#endif  // ifndef __FILE_API__
