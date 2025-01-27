/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/Semaphore.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Unlocks the semaphore.
	/***********************************************************************************/
	Bool Semaphore::Unlock() noexcept
	{
		if (fLockingProcess)
		{
			fLockingProcess			= UserProcess();
			fLockingProcess.Status = ProcessStatusKind::kFrozen;
			return Yes;
		}

		return No;
	}

	/***********************************************************************************/
	/// @brief Locks process in the semaphore.
	/***********************************************************************************/
	Bool Semaphore::Lock(UserProcess& process)
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
		return fLockingProcess->Status == ProcessStatusKind::kRunning;
	}

	/***********************************************************************************/
	/// @brief Try lock or wait.
	/***********************************************************************************/
	Bool Semaphore::LockOrWait(UserProcess& process, TimerInterface* timer)
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
