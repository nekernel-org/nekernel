/* -------------------------------------------

	Copyright (C) 2024, t& Corporation, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <KernelKit/Timer.h>
#include <CompilerKit/CompilerKit.h>

namespace Kernel
{
	class UserThread;

	typedef UserThread& UserProcessRef;

	/// @brief Access control class, which locks a task until one is done.
	class Semaphore final
	{
	public:
		explicit Semaphore() = default;
		~Semaphore()		 = default;

	public:
		bool IsLocked() const;
		bool Unlock() noexcept;

	public:
		void WaitForProcess() noexcept;

	public:
		bool Lock(UserThread& process);
		bool LockOrWait(UserThread& process, TimerInterface* timer);

	public:
		ZKA_COPY_DEFAULT(Semaphore);

	private:
		UserProcessRef fLockingProcess;
	};
} // namespace Kernel
