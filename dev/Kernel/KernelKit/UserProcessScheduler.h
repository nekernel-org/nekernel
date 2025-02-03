/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

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

#define kProcessInvalidID (-1)
#define kProcessNameLen	  (128U)

////////////////////////////////////////////////////
// The current date is: Thu 11/28/2024			  //
////////////////////////////////////////////////////

namespace Kernel
{
	//! @note Forward class declarations.

	class IDylibObject;
	class UserProcess;
	class UserProcessTeam;
	class UserProcessScheduler;
	class UserProcessHelper;

	typedef UInt64 PTime;

	//! @brief Local Process identifier.
	typedef Int64 ProcessID;

	//! @brief Local Process status enum.
	enum class ProcessStatusKind : Int32
	{
		kInvalid,
		kStarting,
		kRunning,
		kKilled,
		kFrozen,
		kFinished,
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

	/// @brief Helper type to describe a code image.
	using ImagePtr = VoidPtr;

	struct UserProcessImage final
	{
		explicit UserProcessImage() = default;

		ImagePtr fCode;
		ImagePtr fBlob;

		Bool HasCode()
		{
			return this->fCode != nullptr;
		}

		Bool HasImage()
		{
			return this->fBlob != nullptr;
		}
	};

	/// @name UserProcess
	/// @brief User process class, holds information about the running process/thread.
	class UserProcess final
	{
	public:
		explicit UserProcess();
		~UserProcess();

	public:
		NE_COPY_DEFAULT(UserProcess);

	public:
		Char			   Name[kProcessNameLen] = {"Process"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemInvalid};
		User*			   Owner{nullptr};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity{AffinityKind::kStandard};
		ProcessStatusKind  Status{ProcessStatusKind::kFinished};
		UInt8*			   StackReserve{nullptr};
		UserProcessImage   Image{};
		SizeT			   StackSize{kSchedMaxStackSz};
		IDylibObject*	   DylibDelegate{nullptr};
		SizeT			   MemoryCursor{0UL};
		SizeT			   MemoryLimit{kSchedMaxMemoryLimit};
		SizeT			   UsedMemory{0UL};

		struct ProcessMemoryHeapList final
		{
			VoidPtr MemoryEntry{nullptr};
			SizeT	MemoryEntrySize{0UL};
			SizeT	MemoryEntryPad{0UL};

			struct ProcessMemoryHeapList* MemoryPrev{nullptr};
			struct ProcessMemoryHeapList* MemoryNext{nullptr};
		};

		struct UserProcessSignal final
		{
			UIntPtr			  SignalArg;
			ProcessStatusKind PreviousStatus;
			UIntPtr			  SignalID;
		};

		UserProcessSignal	   ProcessSignal;
		ProcessMemoryHeapList* ProcessMemoryHeap{nullptr};
		UserProcessTeam*	   ProcessParentTeam;

		VoidPtr VMRegister{0UL};

		enum
		{
			kInvalidExecutableKind,
			kExectuableKind,
			kExectuableDylibKind,
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
		template <typename T>
		Boolean Delete(ErrorOr<T*> ptr, const SizeT& sz);

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
		explicit UserProcessTeam();
		~UserProcessTeam() = default;

		NE_COPY_DEFAULT(UserProcessTeam);

		Array<UserProcess, kSchedProcessLimitPerTeam>& AsArray();
		Ref<UserProcess>&							   AsRef();
		ProcessID&									   Id() noexcept;

	public:
		Array<UserProcess, kSchedProcessLimitPerTeam> mProcessList;
		Ref<UserProcess>							  mCurrentProcess;
		ProcessID									  mTeamId{0};
		ProcessID									  mProcessCount{0};
	};

	typedef Array<UserProcess, kSchedProcessLimitPerTeam> UserThreadArray;

	using UserProcessRef = UserProcess&;

	/// @brief Process scheduler class.
	/// The main class which you call to schedule user processes.
	class UserProcessScheduler final : public ISchedulable
	{
		friend class UserProcessHelper;

	public:
		explicit UserProcessScheduler()	 = default;
		~UserProcessScheduler() override = default;

		NE_COPY_DEFAULT(UserProcessScheduler)

			 operator bool();
		bool operator!();

	public:
		UserProcessTeam& CurrentTeam();

	public:
		ProcessID  Spawn(const Char* name, VoidPtr code, VoidPtr image);
		const Bool Remove(ProcessID process_id);

		const Bool IsUser() override;
		const Bool IsKernel() override;
		const Bool HasMP() override;

	public:
		Ref<UserProcess>& CurrentProcess();
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
#include <KernelKit/UserProcessScheduler.inl>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef INC_PROCESS_SCHEDULER_H */
