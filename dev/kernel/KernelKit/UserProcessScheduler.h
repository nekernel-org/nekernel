/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef INC_PROCESS_SCHEDULER_H
#define INC_PROCESS_SCHEDULER_H

/// @file UserProcessScheduler.h
/// @brief User UserProcess scheduler code and definitions.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#include <ArchKit/ArchKit.h>
#include <KernelKit/LockDelegate.h>
#include <KernelKit/User.h>
#include <NewKit/MutableArray.h>
#include <KernelKit/ProcessSchedulerCore.h>

#define kSchedMinMicroTime		  (AffinityKind::kStandard)
#define kSchedInvalidPID		  (-1)
#define kSchedProcessLimitPerTeam (32U)
#define kSchedTeamCount (512U)

#define kSchedMaxMemoryLimit gib_cast(128) /* max physical memory limit */
#define kSchedMaxStackSz	 mib_cast(8)   /* maximum stack size */

#define kSchedNameLen (128U)

////////////////////////////////////////////////////
// Last revision date is: Fri Mar 28 2025		  //
////////////////////////////////////////////////////

namespace Kernel
{
	//! @brief Forward declarations.

	class IDylibObject;
	class UserProcessScheduler;
	class UserProcessHelper;

	/***********************************************************************************/
	/// @name UserProcess
	/// @brief UserProcess class, holds information about the running process/thread.
	/***********************************************************************************/
	class UserProcess final
	{
	public:
		explicit UserProcess();
		~UserProcess();

	public:
		NE_COPY_DEFAULT(UserProcess)

	public:
		Char			   Name[kSchedNameLen] = {"UserProcess"};
		ProcessSubsystem   SubSystem{ProcessSubsystem::kProcessSubsystemInvalid};
		User*			   Owner{nullptr};
		HAL::StackFramePtr StackFrame{nullptr};
		AffinityKind	   Affinity{AffinityKind::kStandard};
		ProcessStatusKind  Status{ProcessStatusKind::kFinished};
		UInt8*			   StackReserve{nullptr};
		ProcessImage	   Image{};
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

		struct ProcessSignal final
		{
			UIntPtr			  SignalArg;
			ProcessStatusKind PreviousStatus;
			UIntPtr			  SignalID;
		};

		ProcessSignal		   ProcessSignal;
		ProcessMemoryHeapList* ProcessMemoryHeap{nullptr};
		UserProcessTeam*		   ProcessParentTeam;

		VoidPtr VMRegister{0UL};

		enum
		{
			kInvalidExecutableKind,
			kExecutableKind,
			kExecutableDylibKind,
			kExecutableKindCount,
		};

		ProcessTime PTime{0}; //! @brief UserProcess allocated tine.

		PID	  ProcessId{kSchedInvalidPID};
		Int32 Kind{kExecutableKind};

	public:
		/***********************************************************************************/
		//! @brief boolean operator, check status.
		/***********************************************************************************/
		operator bool();

		/***********************************************************************************/
		///! @brief Crashes the app, exits with code ~0.
		/***********************************************************************************/
		Void Crash();

		/***********************************************************************************/
		///! @brief Exits the app.
		/***********************************************************************************/
		Void Exit(const Int32& exit_code = 0);

		/***********************************************************************************/
		///! @brief TLS allocate.
		///! @param sz size of data structure.
		///! @param pad_amount amount to add after pointer.
		///! @return A wrapped pointer, or error code.
		/***********************************************************************************/
		ErrorOr<VoidPtr> New(SizeT sz, SizeT pad_amount = 0);

		/***********************************************************************************/
		///! @brief TLS free.
		///! @param ptr the pointer to free.
		///! @param sz the size of it.
		/***********************************************************************************/
		template <typename T>
		Boolean Delete(ErrorOr<T*> ptr);

		/***********************************************************************************/
		///! @brief Wakes up thread.
		/***********************************************************************************/
		Void Wake(Bool wakeup = false);

	public:
		/***********************************************************************************/
		//! @brief Gets the local exit code.
		/***********************************************************************************/
		const UInt32& GetExitCode() noexcept;

		/***********************************************************************************/
		///! @brief Get the process's name
		///! @example 'C Runtime Library'
		/***********************************************************************************/
		const Char* GetName() noexcept;

		/***********************************************************************************/
		//! @brief return local error code of process.
		//! @return Int32 local error code.
		/***********************************************************************************/
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

		NE_COPY_DEFAULT(UserProcessTeam)

		Array<UserProcess, kSchedProcessLimitPerTeam>& AsArray();
		Ref<UserProcess>&							   AsRef();
		ProcessID&								   Id() noexcept;

	public:
		Array<UserProcess, kSchedProcessLimitPerTeam> mProcessList;
		Ref<UserProcess>							  mCurrentProcess;
		ProcessID								  mTeamId{0};
		ProcessID								  mProcessCount{0};
	};

	typedef Array<UserProcess, kSchedProcessLimitPerTeam> UserThreadArray;

	using UserProcessRef = UserProcess&;

	/***********************************************************************************/
	/// @brief UserProcess scheduler class.
	/// The main class which you call to schedule user processes.
	/***********************************************************************************/
	class UserProcessScheduler final : public ISchedulable
	{
		friend class UserProcessHelper;

	public:
		explicit UserProcessScheduler()	 = default;
		~UserProcessScheduler() override = default;

		NE_COPY_DELETE(UserProcessScheduler)
		NE_MOVE_DELETE(UserProcessScheduler)

			 operator bool();
		bool operator!();

	public:
		UserProcessTeam& CurrentTeam();
		BOOL		 SwitchTeam(UserProcessTeam& team);

	public:
		ProcessID Spawn(const Char* name, VoidPtr code, VoidPtr image);
		Void	  Remove(ProcessID process_id);

		Bool IsUser() override;
		Bool IsKernel() override;
		Bool HasMP() override;

	public:
		Ref<UserProcess>& CurrentProcess();
		SizeT		  Run() noexcept;

	public:
		STATIC UserProcessScheduler& The();

	private:
		UserProcessTeam mTeam{};
	};

	/***********************************************************************************/
	/**
	 * \brief UserProcess helper class, which contains needed utilities for the scheduler.
	 */
	/***********************************************************************************/

	class UserProcessHelper final
	{
	public:
		STATIC Bool Switch(VoidPtr image_ptr, UInt8* stack_ptr, HAL::StackFramePtr frame_ptr, PID new_pid);
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
