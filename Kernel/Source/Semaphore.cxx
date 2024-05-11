/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/Semaphore.hpp>
#include <KernelKit/Timer.hpp>

namespace NewOS
{
	bool Semaphore::Unlock() noexcept
	{
		if (fLockingProcess)
			fLockingProcess = nullptr;

		return fLockingProcess == nullptr;
	}

	bool Semaphore::Lock(ProcessHeader* process)
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

	bool Semaphore::LockOrWait(ProcessHeader* process, const Int64& seconds)
	{
		if (process == nullptr)
			return false;

		HardwareTimer timer(Seconds(seconds));
		timer.Wait();

		return this->Lock(process);
	}

	void Semaphore::Sync() noexcept
	{
		while (fLockingProcess)
		{
		}
	}
} // namespace NewOS
