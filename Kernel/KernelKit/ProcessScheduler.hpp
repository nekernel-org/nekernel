/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#ifndef __PROCESS_SCHEDULER__
#define __PROCESS_SCHEDULER__

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/LockDelegate.hpp>
#include <KernelKit/PermissionSelector.hxx>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/MutableArray.hpp>

#define kSchedMinMicroTime AffinityKind::kHartStandard
#define kSchedInvalidPID   (-1)

#define kSchedProcessLimitPerTeam (100U)

////////////////////////////////////////////////////

// LAST REV: Mon Feb 12 13:52:01 CET 2024

////////////////////////////////////////////////////

namespace NewOS
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
	using ImagePtr = VoidPtr;
	using HeapPtr  = VoidPtr;

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
			MUST_PASS(startImage);
		}

		~ProcessHeader() = default;

		NEWOS_COPY_DEFAULT(ProcessHeader)

	public:
		void SetEntrypoint(UIntPtr& imageStart) noexcept;

	public:
		Char			   Name[kProcessLen] = {"NewOS Process"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::eProcessSubsystemInvalid};
		ProcessSelector	   Selector{ProcessSelector::kRingUser};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity;
		ProcessStatus	   Status;

		// Memory, images.
		HeapPtr	 HeapCursor{nullptr};
		ImagePtr Image{nullptr};
		HeapPtr	 HeapPtr{nullptr};

		// memory usage
		SizeT UsedMemory{0};
		SizeT FreeMemory{0};

		enum
		{
			kAppKind	= 3,
			kLibKind	= 3,
			kDriverKind = 0,
			kKindCount,
		};

		enum
		{
			kRingUserKind	= 3,
			kRingDriverKind = 0,
		};

		ProcessTime PTime;
		PID			ProcessId{kSchedInvalidPID};
		Int32		Ring{kRingDriverKind};
		Int32		Kind{kAppKind};

	public:
		//! @brief boolean operator, check status.
		operator bool()
		{
			return Status != ProcessStatus::kDead;
		}

		//! @brief Crash the app, exits with code ~0.
		void Crash();

		//! @brief Exits app.
		void Exit(Int32 exitCode = 0);

		//! @brief TLS Allocate
		VoidPtr New(const SizeT& sz);

		//! @brief TLS Free.
		Boolean Delete(VoidPtr ptr, const SizeT& sz);

		//! @brief Wakes up threads.
		void Wake(const bool wakeup = false);

		// ProcessHeader getters.
	public:
		//! @brief ProcessHeader name getter, example: "C RunTime"
		const Char* GetName();

		const ProcessSelector& GetSelector();
		const ProcessStatus&   GetStatus();
		const AffinityKind&	   GetAffinity();

	private:
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

	public:
		MutableArray<Ref<ProcessHeader>> mProcessList;
		Ref<ProcessHeader>				 mCurrentProcess;
	};

	using ProcessHeaderRef = ProcessHeader*;

	/// @brief ProcessHeader manager class.
	/// The main class which you call to schedule an app.
	class ProcessScheduler final
	{
	private:
		explicit ProcessScheduler() = default;

	public:
		~ProcessScheduler() = default;

		NEWOS_COPY_DEFAULT(ProcessScheduler)

		operator bool()
		{
			return mTeam.AsArray().Count() > 0;
		}

		bool operator!()
		{
			return mTeam.AsArray().Count() == 0;
		}

	public:
		ProcessTeam& CurrentTeam();

	public:
		SizeT Add(Ref<ProcessHeader>& headerRef);
		bool  Remove(SizeT headerIndex);

	public:
		Ref<ProcessHeader>& GetCurrent();
		SizeT				Run() noexcept;

	public:
		static Ref<ProcessScheduler&> The();

	private:
		ProcessTeam mTeam;
	};

	/*
	 * Just a helper class, which contains some utilities for the scheduler.
	 */

	class ProcessHelper final
	{
	public:
		static bool Switch(HAL::StackFrame* newStack, const PID& newPid);
		static bool CanBeScheduled(Ref<ProcessHeader>& process);
		static PID& GetCurrentPID();
		static bool StartScheduling();
	};

	const Int32& rt_get_exit_code() noexcept;
} // namespace NewOS

#include <KernelKit/ThreadLocalStorage.hxx>

////////////////////////////////////////////////////

// END

////////////////////////////////////////////////////

#endif /* ifndef __PROCESS_SCHEDULER__ */
