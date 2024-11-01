/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/Semaphore.h>

namespace Kernel
{
	Bool Semaphore::Unlock() noexcept
	{
		if (fLockingProcess)
			fLockingProcess = nullptr;
		else
			return No;

		return Yes;
	}

	Bool Semaphore::Lock(UserProcess* process)
	{
		if (!process || fLockingProcess)
			return false;

		fLockingProcess = process;

		return true;
	}

	Bool Semaphore::IsLocked() const
	{
		return fLockingProcess;
	}

	Bool Semaphore::LockOrWait(UserProcess* process, TimerInterface* timer)
	{
		if (process == nullptr)
			return false;

		if (timer == nullptr)
			return false;

		this->Lock(process);

		timer->Wait();

		return this->Lock(process);
	}

	/// @brief Wait with process, either wait for it to be being invalid, or not being run.
	Void Semaphore::WaitForProcess() noexcept
	{
		while (fLockingProcess)
		{
			if (fLockingProcess->GetStatus() != ProcessStatusKind::kRunning)
			{
				this->Unlock();
				break;
			}
		}
	}
} // namespace Kernel
