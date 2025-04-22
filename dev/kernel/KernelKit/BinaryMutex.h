/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <KernelKit/Timer.h>
#include <CompilerKit/CompilerKit.h>

namespace Kernel
{
	class UserProcess;

	typedef UserProcess& UserProcessRef;

	/// @brief Access control class, which locks a task until one is done.
	class BinaryMutex final
	{
	public:
		explicit BinaryMutex() = default;
		~BinaryMutex()		   = default;

	public:
		bool IsLocked() const;
		bool Unlock() noexcept;

	public:
		BOOL WaitForProcess(const Int16& sec) noexcept;

	public:
		bool Lock(UserProcess& process);
		bool LockOrWait(UserProcess& process, TimerInterface* timer);

	public:
		NE_COPY_DEFAULT(BinaryMutex)

	private:
		UserProcessRef fLockingProcess;
	};
} // namespace Kernel
