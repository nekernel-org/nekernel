/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __FILE_API__
#define __FILE_API__

#include <System.Core/Headers/Defs.hxx>

namespace System {
/// @brief System file interface
class FileInterface final {
 public:
  explicit FileInterface(const char *path) {
    mHandle = kInstanceObject->Invoke(kInstanceObject, kProcessCallOpenHandle,
                                      0, path);
  }

  ~FileInterface() {
    kInstanceObject->Invoke(kInstanceObject, kProcessCallCloseHandle, 0,
                            mHandle);
  }

 public:
  CA_COPY_DEFAULT(FileInterface);

 public:
  PtrVoidType Read(UIntPtrType off, SizeType sz) { return (PtrVoidType)kInstanceObject->Invoke(kInstanceObject, mHandle, 2, off, sz);  }
  PtrVoidType Read(SizeType sz) { return (PtrVoidType)kInstanceObject->Invoke(kInstanceObject, mHandle, 3, sz);  }

  void Write(PtrVoidType buf, UIntPtrType off, SizeType sz) { kInstanceObject->Invoke(kInstanceObject, mHandle, 4, buf, off, sz);  }
  void Write(PtrVoidType buf, SizeType sz) { kInstanceObject->Invoke(kInstanceObject, mHandle, 5, buf, sz); }
  
  void Seek(UIntPtrType off) { kInstanceObject->Invoke(kInstanceObject, mHandle, 5); }
  void Rewind() { kInstanceObject->Invoke(kInstanceObject, mHandle, 6); }

 public:
  const char *MIME();
  void MIME(const char *mime);

 private:
  IntPtrType mHandle;
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

}  // namespace System

#endif // ifndef __FILE_API__
