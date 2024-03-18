/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#define Z_SOLO 1

#include <CompilerKit/CompilerKit.hxx>
#include <System.Zip/Defines.hpp>

namespace System::Zip {
class ZipStream;

class ZipStream final {
 public:
  explicit ZipStream();
  ~ZipStream() noexcept;

 public:
  HCORE_COPY_DEFAULT(ZipStream);

 public:
  FilePtr FlushToFile(const char *name);
  void *Deflate(const char *name);
  void Inflate(const char *name, BYTE *data, QWORD sz);

 private:
  VoidPtr fSharedData{nullptr};
  SizeT fSharedSz{0};

};
}  // namespace System.Zip

#define kZipKitMime "application/x-bzip"
