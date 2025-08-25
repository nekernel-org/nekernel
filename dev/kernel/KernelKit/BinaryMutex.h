/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/Timer.h>
#include <NeKit/Defines.h>

namespace Kernel {
class USER_PROCESS;

/// @brief Access control class, which locks a task until one is done.
class BinaryMutex final {
 public:
  explicit BinaryMutex() = default;
  ~BinaryMutex()         = default;

 public:
  bool IsLocked() const;
  bool Unlock() noexcept;

 public:
  BOOL WaitForProcess(const UInt32& sec) noexcept;

 public:
  bool Lock(USER_PROCESS* process);
  bool LockAndWait(USER_PROCESS* process, TimerInterface* timer);

 public:
  NE_COPY_DEFAULT(BinaryMutex)

 private:
  USER_PROCESS* fLockingProcess{nullptr};
};
}  // namespace Kernel
