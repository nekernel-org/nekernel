/* -------------------------------------------

	Copyright (C) 2024, t& Labs, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <CFKit/Property.h>

/***********************************************************************************/
///! @file HardwareThreadScheduler.cc
///! @brief This file handles multi processing in the Kernel.
///! @brief Multi processing is needed for multi-tasking operations.
/***********************************************************************************/

namespace Kernel
{
	/***********************************************************************************/
	/// @note Those symbols are needed in order to switch and validate the stack.
	/***********************************************************************************/

	EXTERN Bool	  hal_check_stack(HAL::StackFramePtr frame_ptr);
	EXTERN_C Bool mp_register_process(HAL::StackFramePtr frame, ProcessID pid);

	STATIC HardwareThreadScheduler kHardwareThreadScheduler;

	///! A HardwareThread class takes care of it's owned hardware thread.
	///! It has a stack for it's core.

	/***********************************************************************************/
	///! @brief C++ constructor.
	/***********************************************************************************/
	HardwareThread::HardwareThread() = default;

	/***********************************************************************************/
	///! @brief C++ destructor.
	/***********************************************************************************/
	HardwareThread::~HardwareThread() = default;

	/***********************************************************************************/
	//! @brief returns the id of the thread.
	/***********************************************************************************/
	const ThreadID& HardwareThread::ID() noexcept
	{
		return fID;
	}

	/***********************************************************************************/
	//! @brief returns the kind of thread we have.
	/***********************************************************************************/
	const ThreadKind& HardwareThread::Kind() noexcept
	{
		return fKind;
	}

	/***********************************************************************************/
	//! @brief is the thread busy?
	//! @return whether the thread is busy or not.
	/***********************************************************************************/
	Bool HardwareThread::IsBusy() noexcept
	{
		STATIC Int64 busy_timer = 0U;

		if (fBusy && busy_timer > this->fPTime)
		{
			busy_timer = 0U;
			fBusy	   = No;
		}

		++busy_timer;

		return fBusy;
	}

	/***********************************************************************************/
	/// @brief Get processor stack frame.
	/***********************************************************************************/

	HAL::StackFramePtr HardwareThread::StackFrame() noexcept
	{
		MUST_PASS(fStack);
		return fStack;
	}

	Void HardwareThread::Busy(const Bool busy) noexcept
	{
		fBusy = busy;
	}

	HardwareThread::operator bool()
	{
		return this->fStack && !this->fBusy;
	}

	/***********************************************************************************/
	/// @brief Wakeup the processor.
	/***********************************************************************************/

	Void HardwareThread::Wake(const bool wakeup) noexcept
	{
		fWakeup = wakeup;
	}

	/***********************************************************************************/
	/// @brief Switch to hardware thread.
	/// @param stack the new hardware thread.
	/// @retval true stack was changed, code is running.
	/// @retval false stack is invalid, previous code is running.
	/***********************************************************************************/
	Bool HardwareThread::Switch(VoidPtr image_ptr, Ptr8 stack_ptr, HAL::StackFramePtr frame, const ThreadID& pid)
	{
		this->fStack	 = frame;
		this->fSourcePID = pid;

		fStack->BP = reinterpret_cast<UIntPtr>(image_ptr);
		fStack->SP = reinterpret_cast<UIntPtr>(stack_ptr);

		Bool ret = mp_register_process(fStack, this->fSourcePID);

		if (ret)
			this->Busy(YES);

		return ret;
	}

	/***********************************************************************************/
	///! @brief Tells if processor is waked up.
	/***********************************************************************************/
	bool HardwareThread::IsWakeup() noexcept
	{
		return fWakeup;
	}

	/***********************************************************************************/
	///! @brief Constructor and destructors.
	///! @brief Default constructor.
	/***********************************************************************************/

	HardwareThreadScheduler::HardwareThreadScheduler() = default;

	/***********************************************************************************/
	///! @brief Default destructor.
	/***********************************************************************************/
	HardwareThreadScheduler::~HardwareThreadScheduler() = default;

	/***********************************************************************************/
	/// @brief Shared singleton function
	/***********************************************************************************/
	HardwareThreadScheduler& HardwareThreadScheduler::The()
	{
		return kHardwareThreadScheduler;
	}

	/***********************************************************************************/
	/// @brief Get Stack Frame of AP.
	/***********************************************************************************/
	HAL::StackFramePtr HardwareThreadScheduler::Leak() noexcept
	{
		return fThreadList[fCurrentThread].fStack;
	}

	/***********************************************************************************/
	/**
	 * Get Hardware thread at index.
	 * @param idx the index
	 * @return the reference to the hardware thread.
	 */
	/***********************************************************************************/
	Ref<HardwareThread*> HardwareThreadScheduler::operator[](const SizeT& idx)
	{
		if (idx == 0)
		{
			if (fThreadList[idx].Kind() != kAPSystemReserved)
			{
				fThreadList[idx].fKind = kAPBoot;
			}
		}
		else if (idx >= kMaxAPInsideSched)
		{
			static HardwareThread* fakeThread = nullptr;
			return {fakeThread};
		}

		return &fThreadList[idx];
	}

	/***********************************************************************************/
	/**
	 * Check if thread pool isn't empty.
	 * @return
	 */
	/***********************************************************************************/
	HardwareThreadScheduler::operator bool() noexcept
	{
		return !fThreadList.Empty();
	}

	/***********************************************************************************/
	/**
	 * Reverse operator bool
	 * @return
	 */
	/***********************************************************************************/
	bool HardwareThreadScheduler::operator!() noexcept
	{
		return fThreadList.Empty();
	}

	/***********************************************************************************/
	/// @brief Returns the amount of core present.
	/// @return the number of APs.
	/***********************************************************************************/
	SizeT HardwareThreadScheduler::Capacity() noexcept
	{
		return fThreadList.Count();
	}
} // namespace Kernel
