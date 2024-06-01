/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessScheduler.cxx
/// @brief MicroKernel process scheduler.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/SMPManager.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <NewKit/String.hpp>
#include <KernelKit/HError.hpp>

///! BUGS: 0

/***********************************************************************************/
/* This file handles the process scheduling.
/***********************************************************************************/

namespace NewOS
{
	/***********************************************************************************/
	/// @brief Exit Code global
	/***********************************************************************************/

	STATIC Int32 kLastExitCode = 0U;

	/// @brief Gets the latest exit code.
	/// @note Not thread-safe.
	const Int32& rt_get_exit_code() noexcept
	{
		return kLastExitCode;
	}

	/***********************************************************************************/
	/// @brief crash current process.
	/***********************************************************************************/

	void ProcessHeader::Crash()
	{
		kcout << this->Name << ": crashed. (id = " << number(kErrorProcessFault);
		kcout << ")\r";

		if (this->Ring != kRingUserKind)
		{
			MUST_PASS(ke_bug_check());
		}

		this->Exit(kErrorProcessFault);
	}

	void ProcessHeader::Wake(const bool should_wakeup)
	{
		this->Status =
			should_wakeup ? ProcessStatus::kRunning : ProcessStatus::kFrozen;
	}

	/***********************************************************************************/

	VoidPtr ProcessHeader::New(const SizeT& sz)
	{
		if (this->FreeMemory < 1)
		{
			DbgLastError() = kErrorHeapOutOfMemory;
			this->Crash(); /// out of memory.

			return nullptr;
		}

		if (this->HeapCursor)
		{
			VoidPtr ptr		 = this->HeapCursor;
			this->HeapCursor = (VoidPtr)((UIntPtr)this->HeapCursor + (sizeof(sz)));

			++this->UsedMemory;
			--this->FreeMemory;

			return ptr;
		}

		return nullptr;
	}

	/***********************************************************************************/

	/* @brief checks if runtime pointer is in region. */
	bool rt_is_in_pool(VoidPtr pool_ptr, VoidPtr pool, const SizeT& sz)
	{
		UIntPtr* _pool_ptr = (UIntPtr*)pool_ptr;
		UIntPtr* _pool	   = (UIntPtr*)pool;

		for (SizeT index = sz; _pool[sz] != kUserHeapMag; --index)
		{
			if (&_pool[index] > &_pool_ptr[sz])
				continue;

			if (_pool[index] == _pool_ptr[index])
				return true;
		}

		return false;
	}

	/* @brief free pointer from usage. */
	Boolean ProcessHeader::Delete(VoidPtr ptr, const SizeT& sz)
	{
		if (sz < 1 || this->HeapCursor == this->HeapPtr)
			return false;

		// also check for the amount of allocations we've done so far.
		if (this->UsedMemory < 1)
			return false;

		if (rt_is_in_pool(ptr, this->HeapCursor, this->UsedMemory))
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
	const Char* ProcessHeader::GetName()
	{
		return this->Name;
	}

	/// @brief process selector getter.
	const ProcessSelector& ProcessHeader::GetSelector()
	{
		return this->Selector;
	}

	/// @brief process status getter.
	const ProcessStatus& ProcessHeader::GetStatus()
	{
		return this->Status;
	}

	/***********************************************************************************/

	/**
	@brief Affinity is the time slot allowed for the process.
	*/
	const AffinityKind& ProcessHeader::GetAffinity()
	{
		return this->Affinity;
	}

	/**
	@brief Standard exit proc.
	*/
	void ProcessHeader::Exit(Int32 exit_code)
	{
		if (this->ProcessId !=
			ProcessScheduler::The().Leak().GetCurrent().Leak().ProcessId)
			ke_stop(RUNTIME_CHECK_PROCESS);

		if (this->Ring == (Int32)ProcessSelector::kRingKernel &&
			ProcessScheduler::The().Leak().GetCurrent().Leak().Ring > 0)
			ke_stop(RUNTIME_CHECK_PROCESS);

		kLastExitCode = exit_code;

		if (this->Ring != (Int32)ProcessSelector::kRingDriver)
		{
			if (this->HeapPtr)
				rt_free_heap(this->HeapPtr);

			this->HeapPtr	 = nullptr;
			this->HeapCursor = nullptr;

			this->FreeMemory = 0UL;
			this->UsedMemory = 0UL;
		}

		//! Delete image if not done already.
		if (this->Image)
			ke_delete_ke_heap(this->Image);
		if (this->StackFrame)
			ke_delete_ke_heap((VoidPtr)this->StackFrame);

		this->Image		 = nullptr;
		this->StackFrame = nullptr;

		ProcessScheduler::The().Leak().Remove(this->ProcessId);
	}

	/// @brief Add process to list.
	/// @param process
	/// @return
	SizeT ProcessScheduler::Add(Ref<ProcessHeader>& process)
	{
		if (!process)
			return -1;

		if (!process.Leak().Image)
		{
			if (process.Leak().Kind != ProcessHeader::kShLibKind)
			{
				return -kErrorNoEntrypoint;
			}
		}

		if (!mTeam.AsArray().Count() > kSchedProcessLimitPerTeam)
			return -kErrorOutOfTeamSlot;

		if (process.Leak().Ring != (Int32)ProcessSelector::kRingKernel)
			return -1;

		kcout << "ProcessScheduler::Add(Ref<ProcessHeader>& process)\r";

		/// Create heap according to type of process.
		if (process.Leak().Kind == ProcessHeader::kAppKind)
			process.Leak().HeapPtr = rt_new_heap(kUserHeapUser | kUserHeapRw);
		else if (process.Leak().Kind == ProcessHeader::kShLibKind)
			process.Leak().HeapPtr = rt_new_heap(kUserHeapUser | kUserHeapRw | kUserHeapShared);
		else
			process.Leak().HeapPtr = rt_new_heap(kUserHeapDriver | kUserHeapRw);

		process.Leak().StackFrame = reinterpret_cast<HAL::StackFrame*>(
			ke_new_ke_heap(sizeof(HAL::StackFrame), true, false));

		MUST_PASS(process.Leak().StackFrame);

		mTeam.AsArray().Add(process);

		process.Leak().ProcessId  = mTeam.AsArray().Count() - 1;
		process.Leak().HeapCursor = process.Leak().HeapPtr;

		return mTeam.AsArray().Count() - 1;
	}

	/// @brief Remove process from list.
	/// @param process
	/// @return
	bool ProcessScheduler::Remove(SizeT process)
	{
		if (process > mTeam.AsArray().Count())
			return false;

		kcout << "ProcessScheduler::Remove(SizeT process)\r";

		return mTeam.AsArray().Remove(process);
	}

	/// @brief Run scheduler.
	/// @return
	SizeT ProcessScheduler::Run() noexcept
	{
		SizeT processIndex = 0; //! we store this guy to tell the scheduler how many
								//! things we have scheduled.

		for (; processIndex < mTeam.AsArray().Count(); ++processIndex)
		{
			auto process = mTeam.AsArray()[processIndex];

			MUST_PASS(process); //! no need for a MUST_PASS(process.Leak());, it is
								//! recursive because of the nature of the class;

			//! run any process needed to be scheduled.
			if (ProcessHelper::CanBeScheduled(process.Leak()))
			{
				auto unwrapped_process = *process.Leak();

				unwrapped_process.PTime = 0;

				// set the current process.
				mTeam.AsRef() = unwrapped_process;

				// tell helper to find a core to schedule on.
				ProcessHelper::Switch(mTeam.AsRef().Leak().StackFrame,
									  mTeam.AsRef().Leak().ProcessId);
			}
			else
			{
				// otherwise increment the P-time.
				++mTeam.AsRef().Leak().PTime;
			}
		}

		return processIndex;
	}

	/// @brief Gets the current scheduled team.
	/// @return
	ProcessTeam& ProcessScheduler::CurrentTeam()
	{
		return mTeam;
	}

	/// @brief Shared instance of the process scheduler.
	/// @return
	Ref<ProcessScheduler&> ProcessScheduler::The()
	{
		static ProcessScheduler ref;
		return {ref};
	}

	/// @brief Gets current running process.
	/// @return
	Ref<ProcessHeader>& ProcessScheduler::GetCurrent()
	{
		return mTeam.AsRef();
	}

	/// @brief Current proccess id getter.
	/// @return Process ID integer.
	PID& ProcessHelper::GetCurrentPID()
	{
		kcout << "ProcessHelper::GetCurrentPID: Leaking ProcessId...\r";
		return ProcessScheduler::The().Leak().GetCurrent().Leak().ProcessId;
	}

	/// @brief Check if process can be schedulded.
	/// @param process the process reference.
	/// @retval true can be schedulded.
	/// @retval false cannot be schedulded.
	bool ProcessHelper::CanBeScheduled(Ref<ProcessHeader>& process)
	{
		if (process.Leak().Status == ProcessStatus::kFrozen ||
			process.Leak().Status == ProcessStatus::kDead)
			return false;

		if (process.Leak().GetStatus() == ProcessStatus::kStarting)
		{
			if (process.Leak().PTime < static_cast<Int>(kSchedMinMicroTime))
			{
				process.Leak().Status	= ProcessStatus::kRunning;
				process.Leak().Affinity = AffinityKind::kHartStandard;

				return true;
			}

			++process.Leak().PTime;
		}

		return process.Leak().PTime > static_cast<Int>(kSchedMinMicroTime);
	}

	/**
	 * @brief Spin scheduler class.
	 */

	bool ProcessHelper::StartScheduling()
	{
		if (ProcessHelper::CanBeScheduled(
				ProcessScheduler::The().Leak().GetCurrent()))
		{
			--ProcessScheduler::The().Leak().GetCurrent().Leak().PTime;
			return false;
		}

		auto processRef = ProcessScheduler::The().Leak();

		if (!processRef)
			return false; // we have nothing to schedule. simply return.

		SizeT ret = processRef.Run();

		kcout << StringBuilder::FromInt(
			"ProcessHelper::StartScheduling() Iterated over {%} jobs inside team.\r", ret);

		return true;
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

		for (SizeT index = 0UL; index < SMPManager::The().Leak().Count(); ++index)
		{
			if (SMPManager::The().Leak()[index].Leak().Kind() == kInvalidHart)
				continue;

			if (SMPManager::The().Leak()[index].Leak().StackFrame() == the_stack)
			{
				SMPManager::The().Leak()[index].Leak().Busy(false);
				continue;
			}

			if (SMPManager::The().Leak()[index].Leak().IsBusy())
				continue;

			if (SMPManager::The().Leak()[index].Leak().Kind() !=
					ThreadKind::kHartBoot &&
				SMPManager::The().Leak()[index].Leak().Kind() !=
					ThreadKind::kHartSystemReserved)
			{
				SMPManager::The().Leak()[index].Leak().Busy(true);
				ProcessHelper::GetCurrentPID() = new_pid;

				return SMPManager::The().Leak()[index].Leak().Switch(the_stack);
			}
		}

		return false;
	}
} // namespace NewOS
