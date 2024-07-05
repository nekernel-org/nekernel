/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#ifndef _INC_PROCESS_SCHEDULER_HXX_
#define _INC_PROCESS_SCHEDULER_HXX_

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/LockDelegate.hpp>
#include <KernelKit/PermissionSelector.hxx>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/MutableArray.hpp>

#define kSchedMinMicroTime (AffinityKind::kHartStandard)
#define kSchedInvalidPID   (-1)

#define kSchedProcessLimitPerTeam (16U)

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

namespace Kernel
{
	class ProcessHeader;
	class ProcessTeam;
	class ProcessScheduler;

	//! @brief Process identifier.
	typedef Int64 ProcessID;

	//! @brief Process name length.
	inline constexpr SizeT kProcessLen = 256U;

	//! @brief Forward declaration.
	class ProcessHeader;
	class ProcessScheduler;
	class ProcessHelper;

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
		kHartStandard = 150,
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
		eProcessSubsystemLogin,
		eProcessSubsystemNative,
		eProcessSubsystemInvalid,
		eProcessSubsystemCount,
	};

	using ProcessSubsystem = ProcessSubsystemEnum;
	using ProcessTime	   = UInt64;
	using PID			   = Int64;

	// for permission manager, tells where we run the code.
	enum class ProcessSelector : Int
	{
		kRingUser,	 /* user ring (or ring 3 in x86) */
		kRingDriver, /* ring 2 in x86, hypervisor privileges in other archs */
		kRingKernel, /* machine privileges */
	};

	// Helper types.
	using ImagePtr	  = VoidPtr;
	using HeapPtrKind = VoidPtr;

	// @name ProcessHeader
	// @brief Process Header (PH)
	// Holds information about the running process.
	// Thread execution is being abstracted away.
	class ProcessHeader final
	{
	public:
		explicit ProcessHeader(VoidPtr startImage = nullptr)
			: Image(startImage)
		{
		}

		~ProcessHeader() = default;

		NEWOS_COPY_DEFAULT(ProcessHeader)

	public:
		void		 SetEntrypoint(UIntPtr& imageStart) noexcept;
		const Int32& GetExitCode() noexcept;

	public:
		Char			   Name[kProcessLen] = {"Process"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::eProcessSubsystemInvalid};
		ProcessSelector	   Selector{ProcessSelector::kRingUser};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity;
		ProcessStatus	   Status;

		// Memory, images.
		HeapPtrKind HeapCursor{nullptr};
		ImagePtr	Image{nullptr};
		HeapPtrKind HeapPtr{nullptr};

		// memory usage
		SizeT UsedMemory{0};
		SizeT FreeMemory{0};

		enum
		{
			kAppKind   = 1,
			kShLibKind = 2,
			kKindCount,
		};

		ProcessTime PTime;
		PID			ProcessId{kSchedInvalidPID};
		Int32		Kind{kAppKind};

	public:
		//! @brief boolean operator, check status.
		operator bool()
		{
			return Status != ProcessStatus::kDead;
		}

		//! @brief Crash the app, exits with code ~0.
		Void Crash();

		//! @brief Exits app.
		Void Exit(Int32 exitCode = 0);

		//! @brief TLS Allocate
		VoidPtr New(const SizeT& sz);

		//! @brief TLS Free.
		Boolean Delete(VoidPtr ptr, const SizeT& sz);

		//! @brief Wakes up threads.
		Void Wake(const bool wakeup = false);

		// ProcessHeader getters.
	public:
		//! @brief ProcessHeader name getter, example: "C RunTime"
		const Char* GetName() noexcept;

		//! @brief return local error code of process.
		//! @return Int32 local error code.
		Int32& GetLocalCode() noexcept;

		const ProcessSelector& GetSelector() noexcept;
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

		MutableArray<Ref<ProcessHeader>>& AsArray();
		Ref<ProcessHeader>&				  AsRef();
		UInt64&							  Id() noexcept;

	public:
		MutableArray<Ref<ProcessHeader>> mProcessList;
		Ref<ProcessHeader>				 mCurrentProcess;
		UInt64							 mTeamId{0};
	};

	using ProcessHeaderRef = ProcessHeader*;

	/// @brief ProcessHeader manager class.
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
		SizeT Add(Ref<ProcessHeader>& headerRef);
		bool  Remove(SizeT headerIndex);

	public:
		Ref<ProcessHeader>& TheCurrent();
		SizeT				Run() noexcept;

	public:
		STATIC Ref<ProcessScheduler&> The();

	private:
		ProcessTeam mTeam;
	};

	/*
	 * Just a helper class, which contains some utilities for the scheduler.
	 */

	class ProcessHelper final
	{
	public:
		STATIC bool Switch(HAL::StackFrame* newStack, const PID& newPid);
		STATIC bool CanBeScheduled(Ref<ProcessHeader>& process);
		STATIC PID&	 TheCurrentPID();
		STATIC SizeT StartScheduling();
	};

	const Int32& rt_get_exit_code() noexcept;
} // namespace Kernel

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef _INC_PROCESS_SCHEDULER_HXX_ */
