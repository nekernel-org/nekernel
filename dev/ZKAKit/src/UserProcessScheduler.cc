/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Incorporated., all rights reserved.

	FILE: UserProcessScheduler.cc
	PURPOSE: EL0/Ring-3 Process scheduler.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessScheduler.cc
/// @brief EL0/Ring-3 process scheduler.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/IPEFDLLObject.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/Heap.h>
#include <NewKit/KString.h>
#include <KernelKit/LPC.h>

///! BUGS: 0

/***********************************************************************************/
/** TODO: Document the Kernel, SDK and kits. */
/***********************************************************************************/

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Exit Code global variable.
	/***********************************************************************************/

	STATIC UInt32 kLastExitCode = 0U;

	/***********************************************************************************/
	/// @brief User Process scheduler global and external reference of thread scheduler.
	/***********************************************************************************/

	STATIC UserProcessScheduler kProcessScheduler;

	UserProcess::UserProcess()	= default;
	UserProcess::~UserProcess() = default;

	/// @brief Gets the last exit code.
	/// @note Not thread-safe.
	/// @return Int32 the last exit code.
	const UInt32& sched_get_exit_code(void) noexcept
	{
		return kLastExitCode;
	}

	/***********************************************************************************/
	/// @brief Crashes the current process->
	/***********************************************************************************/

	Void UserProcess::Crash()
	{
		if (this->Status != ProcessStatusKind::kRunning)
			return;

		kcout << this->Name << ": crashed, error id: " << number(kErrorProcessFault) << endl;
		this->Exit(kErrorProcessFault);
	}

	/***********************************************************************************/
	//! @brief boolean operator, check status.
	/***********************************************************************************/

	UserProcess::operator bool()
	{
		return this->Status == ProcessStatusKind::kRunning;
	}

	/***********************************************************************************/
	/// @brief Gets the local last exit code.
	/// @note Not thread-safe.
	/// @return Int32 the last exit code.
	/***********************************************************************************/

	const UInt32& UserProcess::GetExitCode() noexcept
	{
		return this->fLastExitCode;
	}

	/***********************************************************************************/
	/// @brief Error code variable getter.
	/***********************************************************************************/

	Int32& UserProcess::GetLocalCode() noexcept
	{
		return this->fLocalCode;
	}

	/***********************************************************************************/
	/// @brief Wake process->
	/***********************************************************************************/

	Void UserProcess::Wake(const bool should_wakeup)
	{
		this->Status =
			should_wakeup ? ProcessStatusKind::kRunning : ProcessStatusKind::kFrozen;
	}

	/***********************************************************************************/
	/** @brief Add pointer to entry. */
	/***********************************************************************************/

	ErrorOr<VoidPtr> UserProcess::New(const SizeT& sz, const SizeT& pad_amount)
	{
#ifdef __ZKA_AMD64__
		auto vm_register = hal_read_cr3();
		hal_write_cr3(this->VMRegister);

		auto ptr = mm_new_heap(sz + pad_amount, Yes, Yes);

		hal_write_cr3(vm_register);
#else
		auto ptr = mm_new_heap(sz + pad_amount, Yes, Yes);
#endif

		if (!this->ProcessMemoryHeap)
		{
			this->ProcessMemoryHeap = new UserProcess::UserProcessHeapList();

			this->ProcessMemoryHeap->MemoryEntryPad	 = pad_amount;
			this->ProcessMemoryHeap->MemoryEntrySize = sz;

			this->ProcessMemoryHeap->MemoryEntry = ptr;

			this->ProcessMemoryHeap->MemoryPrev = nullptr;
			this->ProcessMemoryHeap->MemoryNext = nullptr;

			return ErrorOr<VoidPtr>(ptr);
		}
		else
		{
			UserProcessHeapList* entry		= this->ProcessMemoryHeap;
			UserProcessHeapList* prev_entry = nullptr;

			while (!entry)
			{
				if (entry->MemoryEntry == nullptr)
					break; // chose to break here, when we get an already allocated memory entry for our needs.

				prev_entry = entry;
				entry	   = entry->MemoryNext;
			}

			entry->MemoryNext			   = new UserProcessHeapList();
			entry->MemoryNext->MemoryEntry = ptr;

			entry->MemoryNext->MemoryPrev = entry;
			entry->MemoryNext->MemoryNext = nullptr;
		}

		return ErrorOr<VoidPtr>(nullptr);
	}

	/***********************************************************************************/
	/// @brief Gets the name of the current process->
	/***********************************************************************************/

	const Char* UserProcess::GetName() noexcept
	{
		return this->Name;
	}

	/***********************************************************************************/
	/// @brief Gets the owner of the process->
	/***********************************************************************************/

	const User* UserProcess::GetOwner() noexcept
	{
		return this->Owner;
	}

	/// @brief UserProcess status getter.
	const ProcessStatusKind& UserProcess::GetStatus() noexcept
	{
		return this->Status;
	}

	/***********************************************************************************/
	/**
	@brief Affinity is the time slot allowed for the process->
	*/
	/***********************************************************************************/

	const AffinityKind& UserProcess::GetAffinity() noexcept
	{
		return this->Affinity;
	}

	/***********************************************************************************/
	/**
	@brief Exit process method.
	@param exit_code The process's exit code.
	*/
	/***********************************************************************************/

	Void UserProcess::Exit(const Int32& exit_code)
	{
		this->Status		= exit_code > 0 ? ProcessStatusKind::kKilled : ProcessStatusKind::kFrozen;
		this->fLastExitCode = exit_code;

		kLastExitCode = exit_code;

		auto memory_heap_list = this->ProcessMemoryHeap;

#ifdef __ZKA_AMD64__
		auto pd = hal_read_cr3();
		hal_write_cr3(this->VMRegister);
#endif

		// Deleting memory lists. Make sure to free all of them.
		while (memory_heap_list)
		{
			if (memory_heap_list->MemoryEntry)
			{
				MUST_PASS(mm_delete_heap(memory_heap_list->MemoryEntry));
			}

#ifdef __ZKA_AMD64__
			hal_write_cr3(pd);
#endif

			auto next = memory_heap_list->MemoryNext;

			mm_delete_heap(memory_heap_list);

			memory_heap_list = nullptr;
			memory_heap_list = next;
		}

		//! Free the memory's page directory.
		HAL::mm_free_bitmap(this->VMRegister);

		//! Delete image if not done already.
		if (this->Image.fCode && mm_is_valid_heap(this->Image.fCode))
			mm_delete_heap(this->Image.fCode);

		if (this->Image.fBlob && mm_is_valid_heap(this->Image.fBlob))
			mm_delete_heap(this->Image.fBlob);

		if (this->StackFrame && mm_is_valid_heap(this->StackFrame))
			mm_delete_heap((VoidPtr)this->StackFrame);

		this->Image.fBlob = nullptr;
		this->Image.fCode = nullptr;
		this->StackFrame  = nullptr;

		if (this->Kind == kExectuableDLLKind)
		{
			Bool success = false;

			rtl_fini_dll(this, reinterpret_cast<IPEFDLLObject*>(this->PefDLLDelegate), &success);

			if (!success)
			{
				ke_stop(RUNTIME_CHECK_PROCESS);
			}

			this->PefDLLDelegate = nullptr;
		}

		if (this->StackReserve)
			mm_delete_heap(reinterpret_cast<VoidPtr>(this->StackReserve));

		this->ProcessId = 0;
		this->Status	= ProcessStatusKind::kFinished;

		--this->ProcessParentTeam->mProcessCount;

		delete this;
	}

	/***********************************************************************************/
	/// @brief Add process to team.
	/// @param process the process *Ref* class.
	/// @return the process index inside the team.
	/***********************************************************************************/

	ProcessID UserProcessScheduler::Spawn(UserProcess* process)
	{
		if (!process)
		{
			return kProcessInvalidID;
		}

#ifdef __ZKA_AMD64__
		process->VMRegister = mm_new_heap(sizeof(PDE), No, Yes);

		if (!process->VMRegister)
		{
			process->Crash();
			return -kErrorProcessFault;
		}
#endif // __ZKA_AMD64__

		kcout << "Create page directory for: " << process->Name << endl;

		process->StackFrame = reinterpret_cast<HAL::StackFramePtr>(mm_new_heap(sizeof(HAL::StackFrame), Yes, Yes));

		if (!process->StackFrame)
		{
			process->Crash();
			return -kErrorProcessFault;
		}

		kcout << "Create stack for: " << process->Name << endl;

		// Create heap according to type of process->
		if (process->Kind == UserProcess::kExectuableDLLKind)
		{
			process->PefDLLDelegate = rtl_init_dll(process);
			MUST_PASS(process->PefDLLDelegate);

			kcout << "Create DLL Delegate for: " << process->Name << endl;
		}

		process->StackReserve = new UInt8[process->StackSize];

		UInt32 flags = HAL::kMMFlagsPresent;
		flags |= HAL::kMMFlagsWr;
		flags |= HAL::kMMFlagsUser;

		HAL::mm_map_page((VoidPtr)process->StackReserve, flags);

		if (!process->StackReserve)
		{
			process->Crash();
			return -kErrorProcessFault;
		}

		kcout << "Create stack reserve for: " << process->Name << endl;

		ProcessID pid = mTeam.mProcessCount;

		if (pid > kSchedProcessLimitPerTeam)
			return kProcessInvalidID;

		++mTeam.mProcessCount;

		process->ProcessParentTeam = &mTeam;

		process->ProcessId = pid;
		process->Status	   = ProcessStatusKind::kStarting;
		process->PTime	   = (UIntPtr)AffinityKind::kStandard;

		kcout << "Process Name: " << process->Name << endl;
		kcout << "PID: " << number(process->ProcessId) << endl;

		mTeam.mProcessList.Assign(pid, process);

		return process->ProcessId;
	}

	/***********************************************************************************/
	/// @brief Retrieves the singleton of the process scheduler.
	/***********************************************************************************/

	UserProcessScheduler& UserProcessScheduler::The()
	{
		return kProcessScheduler;
	}

	/***********************************************************************************/

	/// @brief Remove process from list.
	/// @param process_id process slot inside team.
	/// @retval true process was removed.
	/// @retval false process doesn't exist in team.

	/***********************************************************************************/

	const Bool UserProcessScheduler::Remove(ProcessID process_id)
	{
		// check if process is within range.
		if (process_id > mTeam.mProcessList.Count())
			return No;

		mTeam.mProcessList[process_id]->Exit(0);

		return Yes;
	}

	const Bool UserProcessScheduler::IsUser()
	{
		return Yes;
	}

	const Bool UserProcessScheduler::IsKernel()
	{
		return No;
	}

	const Bool UserProcessScheduler::HasMP()
	{
		MUST_PASS(kHandoverHeader);
		return kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled;
	}

	/***********************************************************************************/

	/// @brief Run User scheduler object.
	/// @return Process count executed within a team.

	/***********************************************************************************/

	const SizeT UserProcessScheduler::Run() noexcept
	{
		SizeT process_index = 0; //! we store this guy to tell the scheduler how many
								 //! things we have scheduled.

		if (mTeam.mProcessList.Empty())
		{
			kcout << "UserProcessScheduler::Run(): This team doesn't have any process!\r";
			return 0;
		}

		kcout << "UserProcessScheduler::Run(): This team has a process capacity of: " << number(mTeam.mProcessList.Capacity()) << endl;

		for (; process_index < mTeam.AsArray().Capacity(); ++process_index)
		{
			auto process = mTeam.AsArray()[process_index];

			//! check if process needs to be scheduled.
			if (process && UserProcessHelper::CanBeScheduled(process))
			{
				// Set current process header.
				this->GetCurrentProcess() = process;

				process->PTime = static_cast<Int32>(process->Affinity);

				kcout << "Switch to '" << process->Name << "'.\r";

				// tell helper to find a core to schedule on.
				if (!UserProcessHelper::Switch(process->Image.fCode, &process->StackReserve[process->StackSize - 1], process->StackFrame,
											   process->ProcessId))
				{
					process->Crash();
				}
			}
			else
			{
				--process->PTime;
			}
		}

		return process_index;
	}

	/// @brief Gets the current scheduled team.
	/// @return
	UserProcessTeam& UserProcessScheduler::CurrentTeam()
	{
		return mTeam;
	}

	/// @internal

	/// @brief Gets current running process->
	/// @return
	Ref<UserProcess>& UserProcessScheduler::GetCurrentProcess()
	{
		return mTeam.AsRef();
	}

	/// @brief Current proccess id getter.
	/// @return UserProcess ID integer.
	ErrorOr<PID> UserProcessHelper::TheCurrentPID()
	{
		if (!kProcessScheduler.GetCurrentProcess())
			return ErrorOr<PID>{kErrorProcessFault};

		kcout << "UserProcessHelper::TheCurrentPID: Leaking ProcessId...\r";
		return ErrorOr<PID>{kProcessScheduler.GetCurrentProcess().Leak().ProcessId};
	}

	/// @brief Check if process can be schedulded.
	/// @param process the process reference.
	/// @retval true can be schedulded.
	/// @retval false cannot be schedulded.
	Bool UserProcessHelper::CanBeScheduled(const UserProcess* process)
	{
		if (process->Status == ProcessStatusKind::kKilled ||
			process->Status == ProcessStatusKind::kFinished ||
			process->Status == ProcessStatusKind::kFrozen)
			return No;

		if (process->Status == ProcessStatusKind::kInvalid)
			return No;

		if (!process->Image.fCode)
			return No;

		if (!process->Name[0])
			return No;

		return process->PTime < 1;
	}

	/***********************************************************************************/
	/**
	 * @brief Start scheduling current AP.
	 */
	/***********************************************************************************/
	SizeT UserProcessHelper::StartScheduling()
	{
		return kProcessScheduler.Run();
	}

	/***********************************************************************************/
	/**
	 * \brief Does a context switch in a CPU.
	 * \param the_stack the stackframe of the running app.
	 * \param new_pid the process's PID.
	 */
	/***********************************************************************************/

	Bool UserProcessHelper::Switch(VoidPtr image_ptr, UInt8* stack, HAL::StackFramePtr frame_ptr, const PID& new_pid)
	{
		if (!stack || !frame_ptr || !image_ptr || new_pid < 0)
			return No;

		if (!mm_is_valid_heap(image_ptr))
			return No;

		for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Capacity(); ++index)
		{
			if (!HardwareThreadScheduler::The()[index].Leak())
				continue;

			if (HardwareThreadScheduler::The()[index].Leak()->Kind() == kInvalidAP)
				continue;

			if (HardwareThreadScheduler::The()[index].Leak()->Kind() !=
					ThreadKind::kAPBoot &&
				HardwareThreadScheduler::The()[index].Leak()->Kind() !=
					ThreadKind::kAPSystemReserved)
			{
				PID prev_pid									 = UserProcessHelper::TheCurrentPID();
				UserProcessHelper::TheCurrentPID().Leak().Leak() = new_pid;

				////////////////////////////////////////////////////////////
				///	Prepare task switch.								 ///
				////////////////////////////////////////////////////////////

				auto prev_ptime										 = HardwareThreadScheduler::The()[index].Leak()->fPTime;
				HardwareThreadScheduler::The()[index].Leak()->fPTime = UserProcessScheduler::The().CurrentTeam().AsArray()[new_pid]->PTime;
				Bool ret											 = HardwareThreadScheduler::The()[index].Leak()->Switch(image_ptr, stack, frame_ptr, new_pid);

				////////////////////////////////////////////////////////////
				///	Rollback on fail.    								 ///
				////////////////////////////////////////////////////////////
				if (!ret)
				{
					HardwareThreadScheduler::The()[index].Leak()->fPTime = prev_ptime;
					UserProcessHelper::TheCurrentPID().Leak().Leak()	 = prev_pid;

					continue;
				}
			}
		}

		return false;
	}

	/// @brief this checks if any process is on the team.
	UserProcessScheduler::operator bool()
	{
		return mTeam.AsArray().Count() > 0;
	}

	/// @brief this checks if no process is on the team.
	bool UserProcessScheduler::operator!()
	{
		return mTeam.AsArray().Count() == 0;
	}
} // namespace Kernel
