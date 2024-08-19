/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessScheduler.cxx
/// @brief MicroKernel process scheduler.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/PEFSharedObject.hxx>
#include <KernelKit/MP.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/String.hxx>
#include <KernelKit/LPC.hxx>

///! BUGS: 0

/***********************************************************************************/
/* This file handles the process scheduling. */
/***********************************************************************************/

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Exit Code global
	/***********************************************************************************/

	STATIC Int32 cLastExitCode = 0U;

	/// @brief Gets the last exit code.
	/// @note Not thread-safe.
	/// @return Int32 the last exit code.
	const Int32& sched_get_exit_code(void) noexcept
	{
		return cLastExitCode;
	}

	/***********************************************************************************/
	/// @brief crash current process.
	/***********************************************************************************/

	void PROCESS_HEADER_BLOCK::Crash()
	{
		kcout << (*this->Name == 0 ? "Kernel" : this->Name) << ": crashed. (id = ";
		kcout << number(kErrorProcessFault);
		kcout << ")\r";

		if (Kernel::ProcessScheduler::The().Leak().CurrentTeam().AsArray().Count() < 1)
		{
			kcout << "*** BAD PROCESS ***\rTerminating as we are the only process...\r";

			ke_stop(RUNTIME_CHECK_PROCESS);
		}

		this->Exit(kErrorProcessFault);
	}

	/// @brief Gets the local last exit code.
	/// @note Not thread-safe.
	/// @return Int32 the last exit code.
	const Int32& PROCESS_HEADER_BLOCK::GetExitCode() noexcept
	{
		return this->fLastExitCode;
	}

	Int32& PROCESS_HEADER_BLOCK::GetLocalCode() noexcept
	{
		return fLocalCode;
	}

	void PROCESS_HEADER_BLOCK::Wake(const bool should_wakeup)
	{
		this->Status =
			should_wakeup ? ProcessStatus::kRunning : ProcessStatus::kFrozen;
	}

	/***********************************************************************************/

	VoidPtr PROCESS_HEADER_BLOCK::New(const SizeT& sz)
	{
		if (this->HeapCursor)
		{
			if (this->FreeMemory < 1)
			{
				ErrLocal() = kErrorHeapOutOfMemory;

				/* We're going out of memory! crash... */
				this->Crash();

				return nullptr;
			}

			this->HeapCursor = reinterpret_cast<VoidPtr>((UIntPtr)this->HeapCursor + (sizeof(sz)));
			VoidPtr ptr		 = this->HeapCursor;

			++this->UsedMemory;
			--this->FreeMemory;

			return ptr;
		}

		return nullptr;
	}

	/***********************************************************************************/

	/* @brief checks if runtime pointer is in region. */
	bool rt_is_in_pool(VoidPtr pool_ptr, VoidPtr pool, const SizeT& pool_ptr_cur_sz, const SizeT& pool_ptr_used_sz)
	{
		if (pool == nullptr ||
			pool_ptr == nullptr)
			return false;

		UIntPtr* uint_pool_ptr = (UIntPtr*)pool_ptr;
		UIntPtr* uint_pool	   = (UIntPtr*)pool;

		return (UIntPtr)&uint_pool > (UIntPtr)&uint_pool_ptr &&
			   pool_ptr_cur_sz > pool_ptr_used_sz;
	}

	/* @brief free pointer from usage. */
	Boolean PROCESS_HEADER_BLOCK::Delete(VoidPtr ptr, const SizeT& sz)
	{
		if (sz < 1 || this->HeapCursor == this->HeapPtr)
			return false;

		// also check for the amount of allocations we've done so far.
		if (this->UsedMemory < 1)
			return false;

		if (rt_is_in_pool(ptr, this->HeapCursor, this->UsedMemory, this->FreeMemory))
		{
			this->HeapCursor = (VoidPtr)((UIntPtr)this->HeapCursor - (sizeof(sz)));
			rt_zero_memory(ptr, sz);

			++this->FreeMemory;
			--this->UsedMemory;

			return true;
		}

		return false;
	}

	/// @brief process name getter.
	const Char* PROCESS_HEADER_BLOCK::GetProcessName() noexcept
	{
		return this->Name;
	}

	/// @brief process selector getter.
	const ProcessLevelRing& PROCESS_HEADER_BLOCK::GetLevelRing() noexcept
	{
		return this->Selector;
	}

	/// @brief process status getter.
	const ProcessStatus& PROCESS_HEADER_BLOCK::GetStatus() noexcept
	{
		return this->Status;
	}

	/***********************************************************************************/

	/**
	@brief Affinity is the time slot allowed for the process.
	*/
	const AffinityKind& PROCESS_HEADER_BLOCK::GetAffinity() noexcept
	{
		return this->Affinity;
	}

	/**
	@brief Standard exit proc.
	*/
	void PROCESS_HEADER_BLOCK::Exit(const Int32& exit_code)
	{
		if (this->ProcessId !=
			ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId)
			ke_stop(RUNTIME_CHECK_PROCESS);

		fLastExitCode = exit_code;
		cLastExitCode = exit_code;

		//! Delete image if not done already.
		if (this->Image)
			mm_delete_ke_heap(this->Image);

		if (this->StackFrame)
			mm_delete_ke_heap((VoidPtr)this->StackFrame);

		this->Image		 = nullptr;
		this->StackFrame = nullptr;

		if (this->Kind == kSharedObjectKind)
		{
			bool success = false;
			rtl_fini_shared_object(this, this->DLLPtr, &success);

			if (success)
			{
				this->DLLPtr = nullptr;
			}
		}

		ProcessScheduler::The().Leak().Remove(this->ProcessId);
	}

	/// @brief Add process to list.
	/// @param process the process *Ref* class.
	/// @return the process index inside the team.
	SizeT ProcessScheduler::Add(Ref<PROCESS_HEADER_BLOCK>& process)
	{
		if (!process.Leak().Image)
		{
			if (process.Leak().Kind != PROCESS_HEADER_BLOCK::kSharedObjectKind)
			{
				return -kErrorNoEntrypoint;
			}
		}

		if (mTeam.AsArray().Count() > kSchedProcessLimitPerTeam)
			return -kErrorOutOfTeamSlot;

		kcout << "ProcessScheduler:: adding process to team...\r";

		// Create heap according to type of process.
		if (process.Leak().Kind == PROCESS_HEADER_BLOCK::kAppKind)
		{
			process.Leak().HeapPtr = sched_new_heap(kProcessHeapUser | kProcessHeapRw, process.Leak().SizeMemory);
		}
		else if (process.Leak().Kind == PROCESS_HEADER_BLOCK::kSharedObjectKind)
		{
			process.Leak().DLLPtr = rtl_init_shared_object(&process.Leak());
			process.Leak().HeapPtr		 = sched_new_heap(kProcessHeapUser | kProcessHeapRw | kProcessHeapShared, process.Leak().SizeMemory);
		}
		else
		{
			// Something went wrong, do not continue, process may be incorrect.
			process.Leak().Crash();
			return -kErrorProcessFault;
		}

		process.Leak().StackFrame = reinterpret_cast<HAL::StackFrame*>(
			mm_new_ke_heap(sizeof(HAL::StackFrame), Yes, Yes));

		MUST_PASS(process.Leak().StackFrame);

		if (process.Leak().Image)
		{
			process.Leak().StackFrame->BP = reinterpret_cast<HAL::Reg>(process.Leak().Image);
		}
		else
		{
			if (process.Leak().Kind != PROCESS_HEADER_BLOCK::kSharedObjectKind)
			{
				process.Leak().Crash();
				return -kErrorProcessFault;
			}
		}

		if (!process.Leak().StackFrame->SP)
			process.Leak().StackFrame->SP = reinterpret_cast<HAL::Reg>(mm_new_ke_heap(sizeof(UInt8) * 8196, Yes, Yes));

		process.Leak().Status = ProcessStatus::kStarting;

		process.Leak().ProcessId  = (mTeam.AsArray().Count() - 1);
		process.Leak().HeapCursor = process.Leak().HeapPtr;

		MUST_PASS(mTeam.AsArray().Add(process));

		return (mTeam.AsArray().Count() - 1);
	}

	/// @brief Remove process from list.
	/// @param processSlot process slot inside team.
	/// @retval true process was removed.
	/// @retval false process doesn't exist in team.
	Bool ProcessScheduler::Remove(SizeT processSlot)
	{
		// check if process is within range.
		if (processSlot > mTeam.AsArray().Count())
			return false;

		// also check if the process isn't a dummy one.
		if (mTeam.AsArray()[processSlot].Leak().Leak().Image == nullptr)
			return false;

		kcout << "ProcessScheduler: removing process\r";

		return mTeam.AsArray().Remove(processSlot);
	}

	/// @brief Run scheduler.
	/// @return
	SizeT ProcessScheduler::Run() noexcept
	{
		SizeT process_index = 0; //! we store this guy to tell the scheduler how many
								 //! things we have scheduled.

		for (; process_index < mTeam.AsArray().Count(); ++process_index)
		{
			auto process = mTeam.AsArray()[process_index];

			//! check if process needs to be scheduled.
			if (ProcessHelper::CanBeScheduled(process.Leak()))
			{
				auto unwrapped_process = *process.Leak();

				// set the current process.
				mTeam.AsRef() = unwrapped_process;

				// tell helper to find a core to schedule on.
				ProcessHelper::Switch(unwrapped_process.StackFrame,
									  unwrapped_process.ProcessId);

				unwrapped_process.PTime = static_cast<Int32>(unwrapped_process.Affinity);

				kcout << unwrapped_process.Name << ": has been switched to process core.\r";
			}
			else
			{
				// otherwise increment the P-time.
				--mTeam.AsRef().Leak().PTime;
			}
		}

		return process_index;
	}

	/// @brief Gets the current scheduled team.
	/// @return
	ProcessTeam& ProcessScheduler::CurrentTeam()
	{
		return mTeam;
	}

	/// @internal
	STATIC Ref<ProcessScheduler> cSchedulerRef;

	/// @brief Shared instance of the process scheduler.
	/// @return
	Ref<ProcessScheduler>& ProcessScheduler::The()
	{
		return cSchedulerRef;
	}

	/// @brief Gets current running process.
	/// @return
	Ref<PROCESS_HEADER_BLOCK>& ProcessScheduler::TheCurrent()
	{
		return mTeam.AsRef();
	}

	/// @brief Current proccess id getter.
	/// @return Process ID integer.
	PID& ProcessHelper::TheCurrentPID()
	{
		kcout << "ProcessHelper::TheCurrentPID: Leaking ProcessId...\r";
		return ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId;
	}

	/// @brief Check if process can be schedulded.
	/// @param process the process reference.
	/// @retval true can be schedulded.
	/// @retval false cannot be schedulded.
	bool ProcessHelper::CanBeScheduled(Ref<PROCESS_HEADER_BLOCK>& process)
	{
		if (process.Leak().Status == ProcessStatus::kFrozen ||
			process.Leak().Status == ProcessStatus::kDead)
			return false;

		if (process.Leak().Kind == PROCESS_HEADER_BLOCK::kSharedObjectKind)
		{
			if (auto start = process.Leak().DLLPtr->Load<VoidPtr>(kPefStart, rt_string_len(kPefStart), kPefCode);
				start)
			{
				process.Leak().Image = start;
				process.Leak().StackFrame->BP = reinterpret_cast<HAL::Reg>(start);
			}
		}

		if (process.Leak().GetStatus() == ProcessStatus::kStarting)
		{
			if (process.Leak().PTime <= 0)
			{
				process.Leak().Status	= ProcessStatus::kRunning;
				process.Leak().Affinity = AffinityKind::kStandard;

				return true;
			}

			++process.Leak().PTime;
		}

		return process.Leak().PTime > 0;
	}

	/**
	 * @brief Spin scheduler class.
	 */

	SizeT ProcessHelper::StartScheduling()
	{
		auto& process_ref = ProcessScheduler::The().Leak();
		SizeT ret		  = process_ref.Run();

		return ret;
	}

	/**
	 * \brief Does a context switch in a CPU.
	 * \param the_stack the stackframe of the running app.
	 * \param new_pid the process's PID.
	 */

	bool ProcessHelper::Switch(HAL::StackFrame* the_stack, const PID& new_pid)
	{
		if (!the_stack || new_pid < 0)
			return false;

		for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Leak().Count(); ++index)
		{
			if (HardwareThreadScheduler::The().Leak()[index].Leak()->Kind() == kInvalidHart)
				continue;

			if (HardwareThreadScheduler::The().Leak()[index].Leak()->StackFrame() == the_stack)
			{
				HardwareThreadScheduler::The().Leak()[index].Leak()->Busy(false);
				continue;
			}

			if (HardwareThreadScheduler::The().Leak()[index].Leak()->IsBusy())
				continue;

			if (HardwareThreadScheduler::The().Leak()[index].Leak()->Kind() !=
					ThreadKind::kHartBoot &&
				HardwareThreadScheduler::The().Leak()[index].Leak()->Kind() !=
					ThreadKind::kHartSystemReserved)
			{
				HardwareThreadScheduler::The().Leak()[index].Leak()->Busy(true);
				ProcessHelper::TheCurrentPID() = new_pid;

				return HardwareThreadScheduler::The().Leak()[index].Leak()->Switch(the_stack);
			}
		}

		return false;
	}

	/// @brief this checks if any process is on the team.
	ProcessScheduler::operator bool()
	{
		return mTeam.AsArray().Count() > 0;
	}

	/// @brief this checks if no process is on the team.
	bool ProcessScheduler::operator!()
	{
		return mTeam.AsArray().Count() == 0;
	}
} // namespace Kernel
