/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <KernelKit/Timer.hpp>
#include <CompilerKit/CompilerKit.hxx>

namespace Kernel
{
	class ProcessHeader;

	typedef ProcessHeader* ProcessHeaderRef;

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
		bool Lock(ProcessHeader* process);
		bool LockOrWait(ProcessHeader* process, HardwareTimerInterface* timer);

	public:
		NEWOS_COPY_DEFAULT(Semaphore);

	private:
		ProcessHeaderRef fLockingProcess{nullptr};
	};
} // namespace Kernel
