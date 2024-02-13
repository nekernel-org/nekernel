/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#define Z_SOLO 1

#include <CompilerKit/CompilerKit.hpp>
#include <ZipKit/Defines.hpp>
#include <ZipKit/zlib.hpp>

namespace ZipKit {
class ZipStream;

class ZipStream final {
 public:
  explicit ZipStream();
  ~ZipStream() noexcept;

 public:
  HCORE_COPY_DEFAULT(ZipStream);

 public:
  HFilePtr FlushToFile(const char *name);
  void *Deflate(const char *name);
  void Inflate(const char *name, void *data);

 private:
  VoidPtr fSharedData{nullptr};
  SizeT fSharedSz{0};

 private:
  z_stream fStream;
};
}  // namespace ZipKit

#define kZipKitMime "application/x-bzip"
