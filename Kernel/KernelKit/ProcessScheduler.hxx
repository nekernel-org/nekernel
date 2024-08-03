/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef _INC_PROCESS_SCHEDULER_HXX_
#define _INC_PROCESS_SCHEDULER_HXX_

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/LockDelegate.hxx>
#include <KernelKit/User.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/MutableArray.hxx>

#define kSchedMinMicroTime (AffinityKind::kStandard)
#define kSchedInvalidPID   (-1)

#define kSchedProcessLimitPerTeam (16U)

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

namespace Kernel
{
	//! @brief Forward declarations.
	struct PROCESS_HEADER_BLOCK;
	
	class PEFSharedObjectInterface;
	class ProcessTeam;
	class ProcessScheduler;
	class ProcessHelper;

	//! @brief Process identifier.
	typedef Int64 ProcessID;

	//! @brief Process name length.
	inline constexpr SizeT kProcessLen = 256U;


	//! @brief Process status enum.
	enum class ProcessStatus : Int32
	{
		kStarting,
		kRunning,
		kKilled,
		kFrozen,
		kDead
	};

	//! @brief Affinity is the amount of nano-seconds this process is going
	//! to run.
	enum class AffinityKind : Int32
	{
		kInvalid	  = 300,
		kVeryHigh	  = 250,
		kHigh		  = 200,
		kStandard = 150,
		kLowUsage	  = 100,
		kVeryLowUsage = 50,
	};

	// operator overloading.

	inline bool operator<(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int < rhs_int;
	}

	inline bool operator>(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int > rhs_int;
	}

	inline bool operator<=(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int <= rhs_int;
	}

	inline bool operator>=(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int >= rhs_int;
	}

	// end of operator overloading.

	enum ProcessSubsystemEnum
	{
		eProcessSubsystemSecurity,
		eProcessSubsystemNative,
		eProcessSubsystemInvalid,
		eProcessSubsystemCount,
	};

	using ProcessSubsystem = ProcessSubsystemEnum;
	using ProcessTime	   = UInt64;
	using PID			   = Int64;

	// for permission manager, tells where we run the code.
	enum class ProcessLevelRing : Int32
	{
		kRingStdUser   = 1,
		kRingSuperUser = 2,
		kRingGuestUser = 5,
		kRingCount	   = 5,
	};

	// Helper types.
	using ImagePtr	  = VoidPtr;
	using HeapPtrKind = VoidPtr;

	/// @name PROCESS_HEADER_BLOCK
	/// @brief Process Header (PHB). Holds information about the running process. Thread execution the THREAD_INFORMATION_BLOCK.
	struct PROCESS_HEADER_BLOCK final
	{
	public:
		explicit PROCESS_HEADER_BLOCK(VoidPtr startImage = nullptr)
			: Image(startImage)
		{
		}

		~PROCESS_HEADER_BLOCK() = default;

		NEWOS_COPY_DEFAULT(PROCESS_HEADER_BLOCK)

	public:
		void		 SetEntrypoint(UIntPtr& imageStart) noexcept;
		const Int32& GetExitCode() noexcept;

	public:
		Char			   Name[kProcessLen] = {"Unknown Process"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::eProcessSubsystemInvalid};
		ProcessLevelRing	   Selector{ProcessLevelRing::kRingStdUser};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity;
		ProcessStatus	   Status;

		// Memory, images.
		HeapPtrKind HeapCursor{nullptr};
		ImagePtr	Image{nullptr};
		HeapPtrKind HeapPtr{nullptr};

		// shared library handle, reserved for kSharedLib only.
		PEFSharedObjectInterface* SharedObjectPEF{nullptr};

		// Memory usage.
		SizeT UsedMemory{0};
		SizeT FreeMemory{0};

		enum
		{
			kAppKind   = 1,
			kSharedObjectKind = 2,
			kKindCount,
		};

		ProcessTime PTime{0};
		PID			ProcessId{kSchedInvalidPID};
		Int32		Kind{kAppKind};

	public:
		//! @brief boolean operator, check status.
		operator bool()
		{
			return Status != ProcessStatus::kDead;
		}

		///! @brief Crashes the app, exits with code ~0.
		Void Crash();

		///! @brief Exits the app.
		Void Exit(const Int32& exit_code = 0);

		///! @brief TLS allocate.
		///! @param sz size of new ptr.
		VoidPtr New(const SizeT& sz);

		///! @brief TLS free.
		///! @param ptr the pointer to free.
		///! @param sz the size of it.
		Boolean Delete(VoidPtr ptr, const SizeT& sz);

		///! @brief Wakes up threads.
		Void Wake(const bool wakeup = false);

		// PROCESS_HEADER_BLOCK getters.
	public:
		///! @brief Get the process's name
		///! @example 'C Runtime Library'
		const Char* GetProcessName() noexcept;

		//! @brief return local error code of process.
		//! @return Int32 local error code.
		Int32& GetLocalCode() noexcept;

		const ProcessLevelRing& GetSelector() noexcept;
		const ProcessStatus&   GetStatus() noexcept;
		const AffinityKind&	   GetAffinity() noexcept;

	private:
		Int32 fLastExitCode{0};
		Int32 fLocalCode{0};

		friend ProcessScheduler;
		friend ProcessHelper;
	};

	/// \brief Processs Team (contains multiple processes inside it.)
	/// Equivalent to a process batch
	class ProcessTeam final
	{
	public:
		explicit ProcessTeam() = default;
		~ProcessTeam()		   = default;

		NEWOS_COPY_DEFAULT(ProcessTeam);

		MutableArray<Ref<PROCESS_HEADER_BLOCK>>& AsArray();
		Ref<PROCESS_HEADER_BLOCK>&				  AsRef();
		UInt64&							  Id() noexcept;

	public:
		MutableArray<Ref<PROCESS_HEADER_BLOCK>> mProcessList;
		Ref<PROCESS_HEADER_BLOCK>				 mCurrentProcess;
		UInt64							 mTeamId{0};
	};

	using ProcessHeaderRef = PROCESS_HEADER_BLOCK*;

	/// @brief PROCESS_HEADER_BLOCK manager class.
	/// The main class which you call to schedule an app.
	class ProcessScheduler final
	{
		explicit ProcessScheduler() = default;

	public:
		~ProcessScheduler() = default;

		NEWOS_COPY_DEFAULT(ProcessScheduler)

		operator bool();
		bool operator!();

	public:
		ProcessTeam& CurrentTeam();

	public:
		SizeT Add(Ref<PROCESS_HEADER_BLOCK>& processRef);
		Bool  Remove(SizeT processSlot);

	public:
		Ref<PROCESS_HEADER_BLOCK>& TheCurrent();
		SizeT				Run() noexcept;

	public:
		STATIC Ref<ProcessScheduler>& The();

	private:
		ProcessTeam mTeam;
	};

	/*
	 * Just a helper class, which contains some utilities for the scheduler.
	 */

	class ProcessHelper final
	{
	public:
		STATIC bool	 Switch(HAL::StackFrame* newStack, const PID& newPid);
		STATIC bool	 CanBeScheduled(Ref<PROCESS_HEADER_BLOCK>& process);
		STATIC PID&	 TheCurrentPID();
		STATIC SizeT StartScheduling();
	};

	const Int32& sched_get_exit_code(void) noexcept;
} // namespace Kernel

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef _INC_PROCESS_SCHEDULER_HXX_ */
