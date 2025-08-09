/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/BinaryMutex.h>
#include <KernelKit/ProcessScheduler.h>

namespace Kernel {
/***********************************************************************************/
/// @brief Unlocks the binary mutex.
/***********************************************************************************/

Bool BinaryMutex::Unlock() noexcept {
  if (fLockingProcess->Status == ProcessStatusKind::kRunning) {
    fLockingProcess = nullptr;

    return Yes;
  }

  return No;
}

/***********************************************************************************/
/// @brief Locks process in the binary mutex.
/***********************************************************************************/

Bool BinaryMutex::Lock(USER_PROCESS* process) {
  if (!process || this->IsLocked()) return No;

  this->fLockingProcess = process;

  return Yes;
}

/***********************************************************************************/
/// @brief Checks if process is locked.
/***********************************************************************************/

Bool BinaryMutex::IsLocked() const {
  return this->fLockingProcess->Status == ProcessStatusKind::kRunning;
}

/***********************************************************************************/
/// @brief Try lock or wait.
/***********************************************************************************/

Bool BinaryMutex::LockOrWait(USER_PROCESS* process, TimerInterface* timer) {
  if (timer == nullptr) return No;

  this->Lock(process);

  timer->Wait();

  return this->Lock(process);
}

/***********************************************************************************/
/// @brief Wait for process **sec** until we check if it's free.
/// @param sec seconds.
/***********************************************************************************/

BOOL BinaryMutex::WaitForProcess(const UInt32& sec) noexcept {
  HardwareTimer hw_timer(rtl_milliseconds(sec));
  hw_timer.Wait();

  return !this->IsLocked();
}
}  // namespace Kernel
