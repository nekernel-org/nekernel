/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#ifndef INC_PROCESS_SCHEDULER_H
#define INC_PROCESS_SCHEDULER_H

#include <ArchKit/ArchKit.h>
#include <KernelKit/LockDelegate.h>
#include <KernelKit/User.h>
#include <NewKit/MutableArray.h>

#define kSchedMinMicroTime		  (AffinityKind::kStandard)
#define kSchedInvalidPID		  (-1)
#define kSchedProcessLimitPerTeam (32U)

#define kSchedMaxMemoryLimit gib_cast(128)
#define kSchedMaxStackSz	 mib_cast(8)

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

namespace Kernel
{
	//! @note Forward declarations.

	class UserProcess;
	class IPEFDLLObject;
	class UserProcessTeam;
	class UserProcessScheduler;
	class UserProcessHelper;

	//! @brief Local Process identifier.
	typedef Int64 ProcessID;

	//! @brief Local Process name length.
	inline constexpr SizeT kProcessNameLen = 4096U;

	//! @brief Local Process status enum.
	enum class ProcessStatusKind : Int32
	{
		kInvalid,
		kStarting,
		kRunning,
		kKilled,
		kFrozen,
		kDead,
		kCount,
	};

	//! @brief Affinity is the amount of nano-seconds this process is going
	//! to run.
	enum class AffinityKind : Int32
	{
		kRealTime	  = 500,
		kVeryHigh	  = 250,
		kHigh		  = 200,
		kStandard	  = 1000,
		kLowUsage	  = 1500,
		kVeryLowUsage = 2000,
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

	enum class ProcessSubsystem : Int32
	{
		kProcessSubsystemSecurity = 100,
		kProcessSubsystemApplication,
		kProcessSubsystemService,
		kProcessSubsystemDriver,
		kProcessSubsystemInvalid = 255,
		kProcessSubsystemCount	 = 4,
	};

	using ProcessTime = UInt64;
	using PID		  = Int64;

	// for permission manager, tells where we run the code.
	enum class ProcessLevelRing : Int32
	{
		kRingStdUser   = 1,
		kRingSuperUser = 2,
		kRingGuestUser = 5,
		kRingCount	   = 5,
	};

	// Helper types.
	using ImagePtr = VoidPtr;

	/// @name UserProcess
	/// @brief User process header.
	/// Holds information about the running process/thread.
	class UserProcess final
	{
	public:
		explicit UserProcess(VoidPtr start_image);
		explicit UserProcess() = default;

		~UserProcess();

	public:
		ZKA_COPY_DEFAULT(UserProcess);

	public:
		Char			   Name[kProcessNameLen] = {"Application Process (Unnamed)"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemInvalid};
		User*			   Owner{nullptr};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity{AffinityKind::kStandard};
		ProcessStatusKind  Status{ProcessStatusKind::kDead};
		UInt8*			   StackReserve{nullptr};
		ImagePtr		   Code{nullptr};
		ImagePtr		   ExecImg{nullptr};
		SizeT			   StackSize{kSchedMaxStackSz};
		IPEFDLLObject*	   PefDLLDelegate{nullptr};
		SizeT			   MemoryCursor{0};
		SizeT			   MemoryLimit{kSchedMaxMemoryLimit};

		struct USER_PROCESS_HEAP final
		{
			VoidPtr MemoryEntry{nullptr};
			SizeT	MemoryEntrySize{0UL};
			SizeT	MemoryEntryPad{0UL};

			struct USER_PROCESS_HEAP* MemoryPrev{nullptr};
			struct USER_PROCESS_HEAP* MemoryNext{nullptr};
		};

		USER_PROCESS_HEAP* MemoryHeap{nullptr};

		VoidPtr VMRegister{0UL};

		enum
		{
			kInvalidExecutableKind,
			kExectuableKind,
			kExectuableDLLKind,
			kExectuableKindCount,
		};

		ProcessTime PTime{0}; //! @brief Process allocated tine.

		PID	  ProcessId{kSchedInvalidPID};
		Int32 Kind{kExectuableKind};

	public:
		//! @brief boolean operator, check status.
		operator bool();

		///! @brief Crashes the app, exits with code ~0.
		Void Crash();

		///! @brief Exits the app.
		Void Exit(const Int32& exit_code = 0);

		///! @brief TLS allocate.
		///! @param sz size of new ptr.
		ErrorOr<VoidPtr> New(const SizeT& sz, const SizeT& pad_amount = 0);

		///! @brief TLS free.
		///! @param ptr the pointer to free.
		///! @param sz the size of it.
		Boolean Delete(ErrorOr<VoidPtr> ptr, const SizeT& sz);

		///! @brief Wakes up threads.
		Void Wake(const Bool wakeup = false);

	public:
		//! @brief Gets the local exit code.
		const UInt32& GetExitCode() noexcept;

		///! @brief Get the process's name
		///! @example 'C Runtime Library'
		const Char* GetName() noexcept;

		//! @brief return local error code of process.
		//! @return Int32 local error code.
		Int32& GetLocalCode() noexcept;

		const User*				 GetOwner() noexcept;
		const ProcessStatusKind& GetStatus() noexcept;
		const AffinityKind&		 GetAffinity() noexcept;

	private:
		UInt32 fLastExitCode{0};
		Int32  fLocalCode{0};

		friend UserProcessScheduler;
		friend UserProcessHelper;
	};

	/// \brief Processs Team (contains multiple processes inside it.)
	/// Equivalent to a process batch
	class UserProcessTeam final
	{
	public:
		explicit UserProcessTeam() = default;
		~UserProcessTeam()		   = default;

		ZKA_COPY_DEFAULT(UserProcessTeam);

		Array<UserProcess, kSchedProcessLimitPerTeam>& AsArray();
		Ref<UserProcess>&							   AsRef();
		ProcessID&									   Id() noexcept;

	public:
		Array<UserProcess, kSchedProcessLimitPerTeam> mProcessList;
		Ref<UserProcess>							  mCurrentProcess;
		ProcessID									  mTeamId{0};
	};

	using UserProcessPtr = UserProcess*;

	/// @brief Process scheduler class.
	/// The main class which you call to schedule user processes.
	class UserProcessScheduler final : public ISchedulerObject
	{
		friend class UserProcessHelper;

	public:
		explicit UserProcessScheduler()	 = default;
		~UserProcessScheduler() override = default;

		ZKA_COPY_DEFAULT(UserProcessScheduler)

		operator bool();
		bool operator!();

	public:
		UserProcessTeam& CurrentTeam();

	public:
		ProcessID  Add(UserProcess* process);
		const Bool Remove(ProcessID process_id);

		const Bool IsUser() override;
		const Bool IsKernel() override;
		const Bool HasMP() override;

	public:
		Ref<UserProcess>& GetCurrentProcess();
		const SizeT		  Run() noexcept;

	public:
		STATIC UserProcessScheduler& The();

	private:
		UserProcessTeam mTeam{};
	};

	/*
	 * \brief UserProcess helper class, which contains needed utilities for the scheduler.
	 */

	class UserProcessHelper final
	{
	public:
		STATIC Bool Switch(VoidPtr image_ptr, UInt8* stack_ptr, HAL::StackFramePtr frame_ptr, const PID& new_pid);
		STATIC Bool CanBeScheduled(const UserProcess& process);
		STATIC ErrorOr<PID> TheCurrentPID();
		STATIC SizeT		StartScheduling();
	};

	const UInt32& sched_get_exit_code(void) noexcept;
} // namespace Kernel

#include <KernelKit/ThreadLocalStorage.h>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef INC_PROCESS_SCHEDULER_H */
