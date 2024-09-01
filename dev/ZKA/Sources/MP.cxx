/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/MP.hxx>
#include <CFKit/Property.hxx>

///! BUGS: 0

///! @file MP.cxx
///! @brief This file handles multi processing in the Kernel.
///! @brief Multi processing is needed for multi-tasking operations.

namespace Kernel
{
	/***********************************************************************************/
	/// @brief MP object container property.
	/***********************************************************************************/

	Property cSMPCoreName;

	///! A HardwareThread class takes care of it's owned hardware thread.
	///! It has a stack for it's core.

	///! @brief C++ constructor.
	HardwareThread::HardwareThread() = default;

	///! @brief C++ destructor.
	HardwareThread::~HardwareThread() = default;

	//! @brief returns the id of the thread.
	const ThreadID& HardwareThread::ID() noexcept
	{
		return fID;
	}

	//! @brief returns the kind of thread we have.
	const ThreadKind& HardwareThread::Kind() noexcept
	{
		return fKind;
	}

	//! @brief is the thread busy?
	Bool HardwareThread::IsBusy() noexcept
	{
		return fBusy;
	}

	/// @brief Get processor stack frame.

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
		return fStack;
	}

	/// @brief Wakeup the processor.

	Void HardwareThread::Wake(const bool wakeup) noexcept
	{
		fWakeup = wakeup;

		if (!fWakeup)
			mp_hang_thread(fStack);
		else
			mp_wakeup_thread(fStack);
	}

	/// @note Those symbols are needed in order to switch and validate the stack.

	EXTERN Bool	  hal_check_stack(HAL::StackFramePtr stackPtr);
	EXTERN_C Bool mp_register_process(HAL::StackFramePtr stackPtr);

	/// @brief Switch to hardware thread.
	/// @param stack the new hardware thread.
	/// @retval true stack was changed, code is running.
	/// @retval false stack is invalid, previous code is running.
	Bool HardwareThread::Switch(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr frame)
	{
		if (!frame ||
			!image ||
			!stack_ptr)
			return false;

		fStack = frame;

		auto ret = mp_register_process(fStack);

		if (!ret)
		{
			mp_do_context_switch_pre();
			return mp_do_context_switch(image, stack_ptr, fStack) != 0;
		}

		return ret;
	}

	///! @brief Tells if processor is waked up.
	bool HardwareThread::IsWakeup() noexcept
	{
		return fWakeup;
	}

	///! @brief Internal Hardware Thread list.
	STATIC HardwareThread cThreadList[cMaxHWThreads];

	///! @brief Constructor and destructors.

	///! @brief Default constructor.
	HardwareThreadScheduler::HardwareThreadScheduler()
	{
		kcout << "newoskrnl.dll: initializing HardwareThreadScheduler." << endl;

		cSMPCoreName.GetKey() += "Property\\MPClass";
		cSMPCoreName.GetValue() = (PropertyId)this;

		kcout << "newoskrnl.dll: initialized HardwareThreadScheduler." << endl;
	}

	///! @brief Default destructor.
	HardwareThreadScheduler::~HardwareThreadScheduler() = default;

	/// @brief Shared singleton function
	HardwareThreadScheduler& HardwareThreadScheduler::The()
	{
		STATIC HardwareThreadScheduler sched;
		return sched;
	}

	/// @brief Get Stack Frame of Core
	HAL::StackFramePtr HardwareThreadScheduler::Leak() noexcept
	{
		return fThreadList[fCurrentThread].fStack;
	}

	/**
	 * Get Hardware thread at index.
	 * @param idx the index
	 * @return the reference to the hardware thread.
	 */
	Ref<HardwareThread*> HardwareThreadScheduler::operator[](const SizeT& idx)
	{
		if (idx == 0)
		{
			if (fThreadList[idx].Kind() != kHartSystemReserved)
			{
				fThreadList[idx].fKind = kHartBoot;
			}
		}
		else if (idx >= cMaxHWThreads)
		{
			static HardwareThread* fakeThread = new HardwareThread();

			if (!fakeThread)
			{
				fakeThread = new HardwareThread();
			}

			fakeThread->fKind = kInvalidHart;

			return {fakeThread};
		}

		return &fThreadList[idx];
	}

	/**
	 * Check if thread pool isn't empty.
	 * @return
	 */
	HardwareThreadScheduler::operator bool() noexcept
	{
		return !fThreadList.Empty();
	}

	/**
	 * Reverse operator bool
	 * @return
	 */
	bool HardwareThreadScheduler::operator!() noexcept
	{
		return fThreadList.Empty();
	}

	/// @brief Returns the amount of core present.
	/// @return the number of cores.
	SizeT HardwareThreadScheduler::Count() noexcept
	{
		return fThreadList.Count();
	}
} // namespace Kernel
