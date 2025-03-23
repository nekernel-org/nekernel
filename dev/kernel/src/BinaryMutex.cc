/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/BinaryMutex.h>

namespace NeOS
{
	/***********************************************************************************/
	/// @brief Unlocks the semaphore.
	/***********************************************************************************/
	Bool BinaryMutex::Unlock() noexcept
	{
		if (fLockingProcess)
		{
			fLockingProcess		   = UserProcess();
			fLockingProcess.Status = ProcessStatusKind::kFrozen;
			return Yes;
		}

		return No;
	}

	/***********************************************************************************/
	/// @brief Locks process in the semaphore.
	/***********************************************************************************/
	Bool BinaryMutex::Lock(UserProcess& process)
	{
		if (!process || fLockingProcess)
			return No;

		fLockingProcess = process;

		return Yes;
	}

	/***********************************************************************************/
	/// @brief Checks if process is locked.
	/***********************************************************************************/
	Bool BinaryMutex::IsLocked() const
	{
		return fLockingProcess.Status == ProcessStatusKind::kRunning;
	}

	/***********************************************************************************/
	/// @brief Try lock or wait.
	/***********************************************************************************/
	Bool BinaryMutex::LockOrWait(UserProcess& process, TimerInterface* timer)
	{
		if (timer == nullptr)
			return No;

		this->Lock(process);

		timer->Wait();

		return this->Lock(process);
	}

	/***********************************************************************************/
	/// @brief Wait for process **sec** until we check if it's free.
	/// @param sec seconds.
	/***********************************************************************************/
	BOOL BinaryMutex::WaitForProcess(const Int16& sec) noexcept
	{
		HardwareTimer hw_timer(rtl_seconds(sec));
		hw_timer.Wait();

		return !this->IsLocked();
	}
} // namespace NeOS
