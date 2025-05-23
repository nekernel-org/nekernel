/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Atom.h>
#include <NeKit/Defines.h>

namespace Kernel {
enum {
  kLockInvalid,
  kLockDone = 200,
  kLockTimedOut,
  kLockCount = 3,
};

/// @brief Lock condition pointer.
typedef Boolean* LockPtr;

/// @brief Locking delegate class, hangs until limit.
/// @tparam N the amount of cycles to wait.
template <SizeT N>
class LockDelegate final {
 public:
  LockDelegate() = delete;

 public:
  explicit LockDelegate(LockPtr expr) {
    auto spin = 0U;

    while (spin < N) {
      if (*expr) {
        fLockStatus | kLockDone;
        break;
      }

      ++spin;
    }

    if (spin > N) fLockStatus | kLockTimedOut;
  }

  ~LockDelegate() = default;

  LockDelegate& operator=(const LockDelegate&) = delete;
  LockDelegate(const LockDelegate&)            = delete;

  bool Done() { return fLockStatus[kLockDone] == kLockDone; }

  bool HasTimedOut() { return fLockStatus[kLockTimedOut] != kLockTimedOut; }

 private:
  Atom<UInt> fLockStatus;
};
}  // namespace Kernel
