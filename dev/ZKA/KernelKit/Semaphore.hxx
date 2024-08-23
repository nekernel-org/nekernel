/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <KernelKit/Timer.hxx>
#include <CompilerKit/CompilerKit.hxx>

namespace Kernel
{
	class PROCESS_HEADER_BLOCK;

	typedef PROCESS_HEADER_BLOCK* ProcessHeaderRef;

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
		bool Lock(PROCESS_HEADER_BLOCK* process);
		bool LockOrWait(PROCESS_HEADER_BLOCK* process, TimerInterface* timer);

	public:
		ZKA_COPY_DEFAULT(Semaphore);

	private:
		ProcessHeaderRef fLockingProcess{nullptr};
	};
} // namespace Kernel
