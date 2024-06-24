/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/SMPManager.hpp>

///! BUGS: 0

///! @file SMPManager.cxx
///! @brief This file handles multi processing in NewOS.
///! @brief Multi processing is needed for multi-tasking operations.

namespace NewOS
{
	///! A HardwareThread class takes care of it's owned hardware thread.
	///! It has a stack for it's core.

	///! @brief constructor
	HardwareThread::HardwareThread() = default;

	///! @brief destructor
	HardwareThread::~HardwareThread() = default;

	//! @brief returns the id

	const ThreadID& HardwareThread::ID() noexcept
	{
		return fID;
	}

	//! @brief returns the kind

	const ThreadKind& HardwareThread::Kind() noexcept
	{
		return fKind;
	}

	//! @brief is the core busy?

	bool HardwareThread::IsBusy() noexcept
	{
		return fBusy;
	}

	/// @brief Get processor stack frame.

	HAL::StackFramePtr HardwareThread::StackFrame() noexcept
	{
		MUST_PASS(fStack);
		return fStack;
	}

	void HardwareThread::Busy(const bool busy) noexcept
	{
		fBusy = busy;
	}

	HardwareThread::operator bool()
	{
		return fStack;
	}

	/// @brief Wakeup the processor.

	void HardwareThread::Wake(const bool wakeup) noexcept
	{
		fWakeup = wakeup;

		if (!fWakeup)
			rt_hang_thread(fStack);
		else
			rt_wakeup_thread(fStack);
	}

	extern bool rt_check_stack(HAL::StackFramePtr stackPtr);

	/// @brief Switch to hardware thread.
	/// @param stack the new hardware thread.
	/// @retval true stack was changed, code is running.
	/// @retval false stack is invalid, previous code is running.
	bool HardwareThread::Switch(HAL::StackFramePtr stack)
	{
		if (!rt_check_stack(stack))
		{
			/// provide 'nullptr' to free the stack frame.
			if (stack == nullptr)
			{
				delete fStack;
				fStack = nullptr;

				return true;
			}

			return false;
		}

		if (fStack)
		{
			delete fStack;
			fStack = nullptr;
		}

		fStack = stack;

		rt_do_context_switch(fStack);

		return true;
	}

	///! @brief Tells if processor is waked up.
	bool HardwareThread::IsWakeup() noexcept
	{
		return fWakeup;
	}

	//! @brief Constructor and destructor

	///! @brief Default constructor.
	SMPManager::SMPManager() = default;

	///! @brief Default destructor.
	SMPManager::~SMPManager() = default;

	/// @brief Shared singleton function
	Ref<SMPManager> SMPManager::The()
	{
		static SMPManager manager;
		return {manager};
	}

	/// @brief Get Stack Frame of Core
	HAL::StackFramePtr SMPManager::Leak() noexcept
	{
		if (fThreadList[fCurrentThread].Leak() &&
			ProcessHelper::TheCurrentPID() ==
				fThreadList[fCurrentThread].Leak().Leak()->fPID)
			return fThreadList[fCurrentThread].Leak().Leak()->fStack;

		return nullptr;
	}

	/// @brief Finds and switch to a free core.
	bool SMPManager::Switch(HAL::StackFramePtr stack)
	{
		if (stack == nullptr)
			return false;

		for (SizeT idx = 0; idx < kMaxHarts; ++idx)
		{
			// stack != nullptr -> if core is used, then continue.
			if (!fThreadList[idx].Leak() ||
				!fThreadList[idx].Leak().Leak()->IsWakeup() ||
				fThreadList[idx].Leak().Leak()->IsBusy())
				continue;

			// to avoid any null deref.
			if (!fThreadList[idx].Leak().Leak()->fStack)
				continue;
			if (fThreadList[idx].Leak().Leak()->fStack->Rsp == 0)
				continue;
			if (fThreadList[idx].Leak().Leak()->fStack->Rbp == 0)
				continue;

			fThreadList[idx].Leak().Leak()->Busy(true);

			fThreadList[idx].Leak().Leak()->fID = idx;

			/// I figured out this:
			/// Allocate stack
			/// Set APIC base to stack
			/// Do stuff and relocate stack based on this code.
			/// - Amlel
			rt_copy_memory(stack, fThreadList[idx].Leak().Leak()->fStack,
						   sizeof(HAL::StackFrame));

			fThreadList[idx].Leak().Leak()->Switch(fThreadList[idx].Leak().Leak()->fStack);

			fThreadList[idx].Leak().Leak()->fPID = ProcessHelper::TheCurrentPID();

			fThreadList[idx].Leak().Leak()->Busy(false);

			return true;
		}

		return false;
	}

	/**
	 * Index Hardware thread
	 * @param idx the index
	 * @return the reference to the hardware thread.
	 */
	Ref<HardwareThread*> SMPManager::operator[](const SizeT& idx)
	{
		if (idx == 0)
		{
			if (fThreadList[idx].Leak().Leak()->Kind() != kHartSystemReserved)
			{
				fThreadList[idx].Leak().Leak()->fKind = kHartBoot;
			}
		}
		else if (idx >= kMaxHarts)
		{
			static HardwareThread* fakeThread = new HardwareThread();

			if (!fakeThread)
			{
				fakeThread = new HardwareThread();
			}

			fakeThread->fKind = kInvalidHart;

			return {fakeThread};
		}

		return fThreadList[idx].Leak();
	}

	/**
	 * Check if thread pool isn't empty.
	 * @return
	 */
	SMPManager::operator bool() noexcept
	{
		return !fThreadList.Empty();
	}

	/**
	 * Reverse operator bool
	 * @return
	 */
	bool SMPManager::operator!() noexcept
	{
		return fThreadList.Empty();
	}

	/// @brief Returns the amount of core present.
	/// @return the number of cores.
	SizeT SMPManager::Count() noexcept
	{
		return fThreadList.Count();
	}
} // namespace NewOS
