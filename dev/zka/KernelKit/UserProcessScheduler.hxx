/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef _INC_PROCESS_SCHEDULER_HXX_
#define _INC_PROCESS_SCHEDULER_HXX_

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/LockDelegate.hxx>
#include <KernelKit/User.hxx>
#include <NewKit/MutableArray.hxx>

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

	//! @brief UserProcess identifier.
	typedef Int64 ProcessID;

	//! @brief UserProcess name length.
	inline constexpr SizeT kProcessLen = 256U;

	//! @brief UserProcess status enum.
	enum class ProcessStatusKind : Int32
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

	enum ProcessSubsystemEnum : Int32
	{
		kProcessSubsystemSecurity = 100,
		kProcessSubsystemApplication,
		kProcessSubsystemService,
		kProcessSubsystemDriver,
		kProcessSubsystemInvalid = 255,
		kProcessSubsystemCount = 4,
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
		Void		  SetImageStart(VoidPtr imageStart) noexcept;
		const UInt32& GetExitCode() noexcept;

	public:
		Char			   Name[kProcessLen] = {"ZKA Process"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemInvalid};
		User*			   Owner{nullptr};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity{AffinityKind::kStandard};
		ProcessStatusKind  Status{ProcessStatusKind::kDead};
		UInt8*			   StackReserve{nullptr};

		//! @brief Code Image.
		ImagePtr Image{nullptr};

		SizeT StackSize{kSchedMaxStackSz};

		//! @brief Shared library handle, reserved for kExectuableDLLKind types of executables only.
		IPEFDLLObject* PefDLLDelegate{nullptr};

		// Memory usage.
		SizeT MemoryCursor{0};
		SizeT MemoryLimit{kSchedMaxMemoryLimit};

		struct PROCESS_MEMORY_ENTRY final
		{
			VoidPtr MemoryEntry;

			struct PROCESS_MEMORY_ENTRY* MemoryPrev;
			struct PROCESS_MEMORY_ENTRY* MemoryNext;
		};

		PROCESS_MEMORY_ENTRY* MemoryEntryList{nullptr};

		UIntPtr MemoryPD{0UL};

		enum
		{
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
		SizeT										  mProcessAmount{0};
		ProcessID									  mTeamId{0};
	};

	using UserProcessPtr = UserProcess*;

	/// @brief UserProcess scheduler class.
	/// The main class which you call to schedule processes.
	class UserProcessScheduler final : public ISchedulerObject
	{
		friend class UserProcessHelper;

	public:
		explicit UserProcessScheduler() = default;
		~UserProcessScheduler() = default;

		ZKA_COPY_DEFAULT(UserProcessScheduler)

			 operator bool();
		bool operator!();

	public:
		UserProcessTeam& CurrentTeam();

	public:
		SizeT Add(UserProcess process);
		Bool  Remove(ProcessID process_id);

		const Bool IsUser() override;
		const Bool IsKernel() override;
		const Bool HasMP() override;


	public:
		Ref<UserProcess>& CurrentProcess();
		SizeT			  Run() noexcept;

	public:
		STATIC UserProcessScheduler& The();

	private:
		UserProcessTeam mTeam;
	};

	/*
	 * \brief UserProcess helper class, which contains needed utilities for the scheduler.
	 */

	class UserProcessHelper final
	{
	public:
		STATIC bool Switch(VoidPtr image_ptr, UInt8* stack_ptr, HAL::StackFramePtr frame_ptr, const PID& new_pid);
		STATIC bool CanBeScheduled(const UserProcess& process);
		STATIC PID&	 TheCurrentPID();
		STATIC SizeT StartScheduling();
		STATIC Bool InitializeScheduler();
	};

	const UInt32& sched_get_exit_code(void) noexcept;
} // namespace Kernel

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef _INC_PROCESS_SCHEDULER_HXX_ */
