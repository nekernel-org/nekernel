/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/Semaphore.hpp>
#include <KernelKit/Timer.hpp>

namespace HCore {
bool Semaphore::Unlock() noexcept {
  if (fLockingProcess) fLockingProcess = nullptr;

  return fLockingProcess == nullptr;
}

bool Semaphore::Lock(Process* process) {
  if (!process || fLockingProcess) return false;

  fLockingProcess = process;

  return true;
}

bool Semaphore::IsLocked() const { return fLockingProcess; }

bool Semaphore::LockOrWait(Process* process, const Int64& seconds) {
  if (process == nullptr) return false;

  HardwareTimer timer(Seconds(seconds));
  timer.Wait();

  return this->Lock(process);
}

void Semaphore::Sync() noexcept {
  while (fLockingProcess) {
  }
}
}  // namespace HCore
