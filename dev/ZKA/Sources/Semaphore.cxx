/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/Semaphore.hxx>

namespace Kernel
{
	bool Semaphore::Unlock() noexcept
	{
		if (fLockingProcess)
			fLockingProcess = nullptr;

		return fLockingProcess == nullptr;
	}

	bool Semaphore::Lock(UserProcess* process)
	{
		if (!process || fLockingProcess)
			return false;

		fLockingProcess = process;

		return true;
	}

	bool Semaphore::IsLocked() const
	{
		return fLockingProcess;
	}

	bool Semaphore::LockOrWait(UserProcess* process, TimerInterface* timer)
	{
		if (process == nullptr)
			return false;

		if (timer == nullptr)
			return false;

		this->Lock(process);

		timer->Wait();

		return this->Lock(process);
	}

	/// @brief Wait with process, either wait for process being invalid, or not being run.
	Void Semaphore::WaitForProcess() noexcept
	{
		while (fLockingProcess)
		{
			if (fLockingProcess->GetStatus() != ProcessStatus::kRunning)
			{
				this->Unlock();
				break;
			}
		}
	}
} // namespace Kernel
