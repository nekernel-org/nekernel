/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef _INC_PROCESS_SCHEDULER_HXX_
#define _INC_PROCESS_SCHEDULER_HXX_

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/LockDelegate.hxx>
#include <KernelKit/User.hxx>
#include <NewKit/MutableArray.hxx>

#define kSchedMinMicroTime (AffinityKind::kStandard)
#define kSchedInvalidPID   (-1)
#define kSchedMaxStackSz (4096) /* Max stack sz */
#define kSchedProcessLimitPerTeam (16U)

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

namespace Kernel
{
	//! @brief Forward declarations.
	struct UserProcess;

	class PEFDLLInterface;
	class ProcessTeam;
	class UserProcessScheduler;
	class ProcessHelper;

	//! @brief UserProcess identifier.
	typedef Int64 ProcessID;

	//! @brief UserProcess name length.
	inline constexpr SizeT kProcessLen = 256U;

	//! @brief UserProcess status enum.
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
		kRealTime	  = 0000,
		kVeryHigh	  = 2500,
		kHigh		  = 2000,
		kStandard	  = 1500,
		kLowUsage	  = 1000,
		kVeryLowUsage = 5000,
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

	/// @name UserProcess
	/// @brief User process block.
	/// Holds information about the running process/thread.
	struct UserProcess final
	{
	public:
		explicit UserProcess(VoidPtr startImage = nullptr)
			: Image(startImage)
		{
		}

		~UserProcess() = default;

		ZKA_COPY_DEFAULT(UserProcess)

	public:
		void		  SetEntrypoint(VoidPtr imageStart) noexcept;
		const UInt32& GetExitCode() noexcept;

	public:
		Char			   Name[kProcessLen] = {"PROCESS #0 (TEAM 0)"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::eProcessSubsystemInvalid};
		User*			   Owner{nullptr};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity{AffinityKind::kStandard};
		ProcessStatus	   Status{ProcessStatus::kDead};
		UInt8* StackReserve{ nullptr };

		// Memory, images pointers.
		HeapPtrKind HeapCursor{nullptr};
		ImagePtr	Image{nullptr};
		HeapPtrKind HeapPtr{nullptr};

		SizeT StackSize{mib_cast(8)};

		// shared library handle, reserved for kDLLKind types of executables only.
		PEFDLLInterface*	  DLLPtr{nullptr};
		UserProcess* Parent{nullptr};

		// Memory usage.
		SizeT UsedMemory{0};
		SizeT FreeMemory{0};
		SizeT SizeMemory{gib_cast(4)};

		enum
		{
			kExeKind		  = 1,
			kDLLKind = 2,
			kKindCount,
		};

		ProcessTime PTime{0};
		PID			ProcessId{kSchedInvalidPID};
		Int32		Kind{kExeKind};

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

		// UserProcess getters.
	public:
		///! @brief Get the process's name
		///! @example 'C Runtime Library'
		const Char* GetProcessName() noexcept;

		//! @brief return local error code of process.
		//! @return Int32 local error code.
		Int32& GetLocalCode() noexcept;

		const User*			 GetOwner() noexcept;
		const ProcessStatus& GetStatus() noexcept;
		const AffinityKind&	 GetAffinity() noexcept;

	private:
		UInt32 fLastExitCode{0};
		Int32  fLocalCode{0};

		friend UserProcessScheduler;
		friend ProcessHelper;
	};

	/// \brief Processs Team (contains multiple processes inside it.)
	/// Equivalent to a process batch
	class ProcessTeam final
	{
	public:
		explicit ProcessTeam() = default;
		~ProcessTeam()		   = default;

		ZKA_COPY_DEFAULT(ProcessTeam);

		Array<UserProcess, kSchedProcessLimitPerTeam>& AsArray();
		Ref<UserProcess>&								AsRef();
		ProcessID&													Id() noexcept;

	public:
		Array<UserProcess, kSchedProcessLimitPerTeam> mProcessList;
		Ref<UserProcess>							   mCurrentProcess;
		SizeT												   mProcessAmount{0};
		ProcessID											   mTeamId{0};
	};

	using UserProcessPtr = UserProcess*;

	/// @brief UserProcess scheduler class.
	/// The main class which you call to schedule processes.
	class UserProcessScheduler final
	{
	public:
		explicit UserProcessScheduler() = default;

		~UserProcessScheduler() = default;

		ZKA_COPY_DEFAULT(UserProcessScheduler)

		operator bool();
		bool operator!();

	public:
		ProcessTeam& CurrentTeam();

	public:
		SizeT Add(UserProcess& processRef);
		Bool  Remove(ProcessID processSlot);

	public:
		Ref<UserProcess>& CurrentProcess();
		SizeT					   Run() noexcept;

	public:
		STATIC UserProcessScheduler& The();

	private:
		ProcessTeam mTeam;
	};

	/*
	 * \brief UserProcess helper class, which contains needed utilities for the scheduler.
	 */

	class ProcessHelper final
	{
	public:
		STATIC bool	 Switch(VoidPtr image_ptr, UInt8* stack_ptr, HAL::StackFramePtr frame_ptr, const PID& new_pid);
		STATIC bool	 CanBeScheduled(UserProcess& process);
		STATIC PID&	 TheCurrentPID();
		STATIC SizeT StartScheduling();
	};

	const UInt32& sched_get_exit_code(void) noexcept;
} // namespace Kernel

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef _INC_PROCESS_SCHEDULER_HXX_ */
