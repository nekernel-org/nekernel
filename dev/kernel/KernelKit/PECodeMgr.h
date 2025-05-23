/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  File: PECodeMgr.h
  Purpose: PE32+ Code Mgr and Dylib mgr.

  Revision History:

  12/02/24: Added file (amlel)

------------------------------------------- */

#pragma once

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

#include <KernelKit/FileMgr.h>
#include <KernelKit/LoaderInterface.h>
#include <KernelKit/PE.h>
#include <NeKit/ErrorOr.h>
#include <NeKit/KString.h>

#ifndef INC_PROCESS_SCHEDULER_H
#include <KernelKit/ProcessScheduler.h>
#endif

#define kPeApplicationMime "application/vnd-portable-executable"

namespace Kernel {
///
/// \name PE32Loader
/// \brief PE32+ loader class.
///
class PE32Loader : public LoaderInterface {
 private:
  explicit PE32Loader() = delete;

 public:
  explicit PE32Loader(const VoidPtr blob);
  explicit PE32Loader(const Char* path);
  ~PE32Loader() override;

 public:
  NE_COPY_DEFAULT(PE32Loader)

 public:
  const Char* Path() override;
  const Char* AsString() override;
  const Char* MIME() override;

 public:
  ErrorOr<VoidPtr> FindStart() override;
  ErrorOr<VoidPtr> FindSymbol(const Char* name, Int32 kind) override;
  ErrorOr<VoidPtr> GetBlob() override;

 public:
  bool IsLoaded() noexcept;

 private:
#ifdef __FSKIT_INCLUDES_NEFS__
  OwnPtr<FileStream<Char, NeFileSystemMgr>> fFile;
#elif defined(__FSKIT_INCLUDES_HEFS__)
  OwnPtr<FileStream<Char, HeFileSystemMgr>> fFile;
#else
  OwnPtr<FileStream<Char>> fFile;
#endif  // __FSKIT_INCLUDES_NEFS__

  Ref<KString> fPath;
  VoidPtr      fCachedBlob;
  bool         fBad;
};
}  // namespace Kernel