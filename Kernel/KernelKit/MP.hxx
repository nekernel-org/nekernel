/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __INC_MP_MANAGER_HPP__
#define __INC_MP_MANAGER_HPP__

#include <ArchKit/ArchKit.hxx>
#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Ref.hxx>

/// @note Last Rev Sun 28 Jul CET 2024
/// @note Last Rev Thu, Aug  1, 2024  9:07:38 AM

#define cMaxHWThreads (8U)

namespace Kernel
{
	class HardwareThread;
	class HardwareThreadScheduler;

	using ThreadID = UInt32;

	enum ThreadKind
	{
		kHartSystemReserved, // System reserved thread, well user can't use it
		kHartStandard,		 // user thread, cannot be used by kernel
		kHartFallback,		 // fallback thread, cannot be used by user if not clear or
							 // used by kernel.
		kHartBoot,			 // The core we booted from, the mama.
		kInvalidHart,
		kHartCount,
	};

	typedef enum ThreadKind SmThreadKind;
	typedef ThreadID		SmThreadID;

	///
	/// \name HardwareThread
	/// \brief Abstraction over the CPU's core, used to run processes or threads.
	///

	class HardwareThread final
	{
	public:
		explicit HardwareThread();
		~HardwareThread();

	public:
		NEWOS_COPY_DEFAULT(HardwareThread)

	public:
		operator bool();

	public:
		void Wake(const bool wakeup = false) noexcept;
		void Busy(const bool busy = false) noexcept;

	public:
		bool Switch(HAL::StackFrame* stack);
		bool IsWakeup() noexcept;

	public:
		HAL::StackFrame*  StackFrame() noexcept;
		const ThreadKind& Kind() noexcept;
		bool			  IsBusy() noexcept;
		const ThreadID&	  ID() noexcept;

	private:
		HAL::StackFrame* fStack{nullptr};
		ThreadKind		 fKind{ThreadKind::kInvalidHart};
		ThreadID		 fID{0};
		ProcessID		 fSourcePID{-1};
		bool			 fWakeup{false};
		bool			 fBusy{false};

	private:
		friend class HardwareThreadScheduler;
	};

	///
	/// \name HardwareThreadScheduler
	/// \brief Class to manage the thread scheduling.
	///

	class HardwareThreadScheduler final
	{
	private:
		explicit HardwareThreadScheduler();

	public:
		~HardwareThreadScheduler();
		NEWOS_COPY_DEFAULT(HardwareThreadScheduler);

	public:
		bool			   Switch(HAL::StackFramePtr the);
		HAL::StackFramePtr Leak() noexcept;

	public:
		Ref<HardwareThread*> operator[](const SizeT& idx);
		bool				 operator!() noexcept;
		operator bool() noexcept;

	public:
		/// @brief Shared instance of the MP Manager.
		/// @return the reference to the mp manager class.
		static Ref<HardwareThreadScheduler> The();

	public:
		/// @brief Returns the amount of threads present in the system.
		/// @returns SizeT the amount of cores present.
		SizeT Count() noexcept;

	private:
		Array<HardwareThread, cMaxHWThreads> fThreadList;
		ThreadID						 fCurrentThread{0};
	};

	/// @brief wakes up thread.
	/// wakes up thread from hang.
	Void mp_wakeup_thread(HAL::StackFramePtr stack);

	/// @brief makes thread sleep.
	/// hooks and hangs thread to prevent code from executing.
	Void mp_hang_thread(HAL::StackFramePtr stack);
} // namespace Kernel

#endif // !__INC_MP_MANAGER_HPP__
