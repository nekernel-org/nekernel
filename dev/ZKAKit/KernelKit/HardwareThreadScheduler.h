/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Incorporated., all rights reserved.

------------------------------------------- */

#ifndef __INC_MP_MANAGER_H__
#define __INC_MP_MANAGER_H__

#include <ArchKit/ArchKit.h>
#include <CompilerKit/CompilerKit.h>
#include <NewKit/Ref.h>

/// @note Last Rev Sun 28 Jul CET 2024
/// @note Last Rev Thu, Aug  1, 2024  9:07:38 AM

#define kMaxAPInsideSched (8U)

namespace Kernel
{
	class HardwareThread;
	class HardwareThreadScheduler;

	using ThreadID = UInt32;

	enum ThreadKind
	{
		kAPSystemReserved, // System reserved thread, well user can't use it
		kAPStandard,	   // user thread, cannot be used by Kernel
		kAPFallback,	   // fallback thread, cannot be used by user if not clear or
						   // used by Kernel.
		kAPBoot,		   // The core we booted from, the mama.
		kInvalidAP,
		kAPCount,
	};

	typedef enum ThreadKind ThreadKind;
	typedef ThreadID		ThreadID;

	/***********************************************************************************/
	///
	/// \name HardwareThread
	/// \brief Abstraction over the CPU's core, used to run processes or threads.
	///
	/***********************************************************************************/

	class HardwareThread final
	{
	public:
		explicit HardwareThread();
		~HardwareThread();

	public:
		ZKA_COPY_DEFAULT(HardwareThread)

	public:
		operator bool();

	public:
		void Wake(const bool wakeup = false) noexcept;
		void Busy(const bool busy = false) noexcept;

	public:
		bool Switch(VoidPtr image, Ptr8 stack_ptr, HAL::StackFramePtr frame, const ProcessID& pid);
		bool IsWakeup() noexcept;

	public:
		HAL::StackFramePtr StackFrame() noexcept;
		const ThreadKind&  Kind() noexcept;
		bool			   IsBusy() noexcept;
		const ThreadID&	   ID() noexcept;

	private:
		HAL::StackFramePtr fStack{nullptr};
		ThreadKind		   fKind{ThreadKind::kAPStandard};
		ThreadID		   fID{0};
		ProcessID		   fSourcePID{-1};
		Bool			   fWakeup{false};
		Bool			   fBusy{false};
		UInt64			   fPTime{0};

	private:
		friend class HardwareThreadScheduler;
		friend class UserProcessHelper;
	};

	///
	/// \name HardwareThreadScheduler
	/// \brief Class to manage the thread scheduling.
	///

	class HardwareThreadScheduler final : public ISchedulable
	{
	private:
		friend class UserProcessHelper;

	public:
		explicit HardwareThreadScheduler();
		~HardwareThreadScheduler();
		ZKA_COPY_DEFAULT(HardwareThreadScheduler);

	public:
		HAL::StackFramePtr Leak() noexcept;

	public:
		Ref<HardwareThread*> operator[](const SizeT& idx);
		bool				 operator!() noexcept;
							 operator bool() noexcept;

		const Bool IsUser() override
		{
			return Yes;
		}

		const Bool IsKernel() override
		{
			return No;
		}

		const Bool HasMP() override
		{
			return kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled;
		}

	public:
		/// @brief Shared instance of the MP Mgr.
		/// @return the reference to the mp manager class.
		STATIC HardwareThreadScheduler& The();

	public:
		/// @brief Returns the amount of threads present in the system.
		/// @returns SizeT the amount of cores present.
		SizeT Capacity() noexcept;

	private:
		Array<HardwareThread, kMaxAPInsideSched> fThreadList;
		ThreadID								 fCurrentThread{0};
	};

	/// @brief wakes up thread.
	/// wakes up thread from hang.
	Void mp_wakeup_thread(HAL::StackFramePtr stack);

	/// @brief makes thread sleep.
	/// hooks and hangs thread to prevent code from executing.
	Void mp_hang_thread(HAL::StackFramePtr stack);
} // namespace Kernel

#endif // !__INC_MP_MANAGER_H__
