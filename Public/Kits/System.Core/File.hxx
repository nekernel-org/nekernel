/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __FILE_API__
#define __FILE_API__

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

/// @brief SOM class, translated to C++

namespace System {
class File final {
 public:
  explicit File(const char *path) {
    mHandle = kInstanceObject->Invoke(kInstanceObject, kProcessCallOpenHandle,
                                      0, path);
  }

  ~File() {
    kInstanceObject->Invoke(kInstanceObject, kProcessCallCloseHandle, 0,
                            mHandle);
  }

 public:
  HCORE_COPY_DEFAULT(File);

 public:
  voidPtr Read(UIntPtr off, SizeT sz) { return (VoidPtr)kInstanceObject->Invoke(kInstanceObject, mHandle, 2, off, sz);  }
  voidPtr Read(SizeT sz) { return (VoidPtr)kInstanceObject->Invoke(kInstanceObject, mHandle, 3, sz);  }

  void Write(VoidPtr buf, UIntPtr off, SizeT sz) { kInstanceObject->Invoke(kInstanceObject, mHandle, 4, buf, off, sz);  }
  void Write(VoidPtr buf, SizeT sz) { kInstanceObject->Invoke(kInstanceObject, mHandle, 5, buf, sz); }
  
  void Seek(UIntPtr off) { kInstanceObject->Invoke(kInstanceObject, mHandle, 5); }
  void Rewind() { kInstanceObject->Invoke(kInstanceObject, mHandle, 6); }

 public:
  const char *MIME();
  void MIME(const char *mime);

 private:
  IntPtr mHandle;
};

typedef File *FilePtr;

/// @brief file exception
/// Throws when the file isn't found or invalid.
class FileException : public SystemException {
 public:
  explicit FileException() = default;
  virtual ~FileException() = default;

 public:
  HCORE_COPY_DEFAULT(FileException);

 public:
  const char *Name() override { return "FileException"; }
  const char *Reason() override { return mReason; }

 private:
  const char *mReason{"System.Core: FileException: Catastrophic failure!"};
};

}  // namespace System

#endif // ifndef __FILE_API__
