/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/Semaphore.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Unlocks process out of the semaphore.
	/***********************************************************************************/
	Bool Semaphore::Unlock() noexcept
	{
		if (fLockingProcess)
			fLockingProcess = UserThread();
		else
			return No;

		return Yes;
	}

	/***********************************************************************************/
	/// @brief Locks process in the semaphore.
	/***********************************************************************************/
	Bool Semaphore::Lock(UserThread& process)
	{
		if (!process || fLockingProcess)
			return No;

		fLockingProcess = process;

		return Yes;
	}

	/***********************************************************************************/
	/// @brief Checks if process is locked.
	/***********************************************************************************/
	Bool Semaphore::IsLocked() const
	{
		return fLockingProcess;
	}

	/***********************************************************************************/
	/// @brief Try lock or wait.
	/***********************************************************************************/
	Bool Semaphore::LockOrWait(UserThread& process, TimerInterface* timer)
	{
		if (timer == nullptr)
			return No;

		this->Lock(process);

		timer->Wait();

		return this->Lock(process);
	}

	/***********************************************************************************/
	/// @brief Wait for process to be free.
	/***********************************************************************************/
	Void Semaphore::WaitForProcess() noexcept
	{
		while (fLockingProcess)
			;
	}
} // namespace Kernel
