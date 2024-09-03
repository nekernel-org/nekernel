/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessScheduler.cxx
/// @brief User UserProcess scheduler.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/PEFDLLInterface.hxx>
#include <KernelKit/MP.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/String.hxx>
#include <KernelKit/LPC.hxx>

///! BUGS: 0

/***********************************************************************************/
/* TODO: Document more the Kernel, sdk and kits. */
/***********************************************************************************/

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Exit Code global variable.
	/***********************************************************************************/

	UInt32 cLastExitCode = 0U;

	/***********************************************************************************/
	/// @brief UserProcess scheduler instance.
	/***********************************************************************************/

	UserProcessScheduler* cProcessScheduler = nullptr;

	/// @brief Gets the last exit code.
	/// @note Not thread-safe.
	/// @return Int32 the last exit code.
	const UInt32& sched_get_exit_code(void) noexcept
	{
		return cLastExitCode;
	}

	/***********************************************************************************/
	/// @brief crash current process.
	/***********************************************************************************/

	Void UserProcess::Crash()
	{
		if (this->Name == 0)
			return;

		kcout << this->Name << ": crashed. (id = " << number(kErrorProcessFault) << endl;

		this->Exit(kErrorProcessFault);

		UserProcessHelper::StartScheduling();
	}

	/// @brief Gets the local last exit code.
	/// @note Not thread-safe.
	/// @return Int32 the last exit code.
	const UInt32& UserProcess::GetExitCode() noexcept
	{
		return this->fLastExitCode;
	}

	/***********************************************************************************/
	/// @brief Error code variable getter.
	/***********************************************************************************/

	Int32& UserProcess::GetLocalCode() noexcept
	{
		return fLocalCode;
	}

	void UserProcess::Wake(const bool should_wakeup)
	{
		this->Status =
			should_wakeup ? ProcessStatusKind::kRunning : ProcessStatusKind::kFrozen;
	}

	/***********************************************************************************/

	VoidPtr UserProcess::New(const SizeT& sz)
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
			VoidPtr cursor	 = this->HeapCursor;

			++this->UsedMemory;
			--this->FreeMemory;

			return cursor;
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
	Boolean UserProcess::Delete(VoidPtr ptr, const SizeT& sz)
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

	/// @brief UserProcess name getter.
	const Char* UserProcess::GetProcessName() noexcept
	{
		return this->Name;
	}

	/// @brief UserProcess user getter.
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
	@brief Affinity is the time slot allowed for the process.
	*/
	const AffinityKind& UserProcess::GetAffinity() noexcept
	{
		return this->Affinity;
	}

	/**
	@brief Standard exit proc.
	*/
	void UserProcess::Exit(const Int32& exit_code)
	{
		this->Status = ProcessStatusKind::kDead;
		
		fLastExitCode = exit_code;
		cLastExitCode = exit_code;

		//! Delete image if not done already.
		if (this->Image)
			mm_delete_ke_heap(this->Image);

		if (this->StackFrame)
			mm_delete_ke_heap((VoidPtr)this->StackFrame);

		this->Image		 = nullptr;
		this->StackFrame = nullptr;

		if (this->Kind == kDLLKind)
		{
			Bool success = false;
			rtl_fini_shared_object(this, this->DLLPtr, &success);

			if (success)
			{
				this->DLLPtr = nullptr;
			}
		}

		if (this->StackReserve)
			delete[] this->StackReserve;

		cProcessScheduler->Remove(this->ProcessId);
	}

	/// @brief Add process to list.
	/// @param process the process *Ref* class.
	/// @return the process index inside the team.
	SizeT UserProcessScheduler::Add(UserProcess& process)
	{
		if (!process.Image)
		{
			return -kErrorInvalidData;
		}

		kcout << "UserProcessScheduler: Adding process to team...\r";

		// Create heap according to type of process.
		if (process.Kind == UserProcess::kExeKind)
		{
			process.HeapPtr = mm_new_ke_heap(process.SizeMemory, true, true);
		}
		else if (process.Kind == UserProcess::kDLLKind)
		{
			process.DLLPtr	= rtl_init_shared_object(&process);
			process.HeapPtr = mm_new_ke_heap(process.SizeMemory, true, true);
		}
		else
		{
			// Something went wrong, do not continue, process may be incorrect.
			process.Crash();
			return -kErrorProcessFault;
		}

		process.StackFrame = new HAL::StackFrame();

		MUST_PASS(process.StackFrame);

		if (process.Image)
		{
			// get preferred stack size by app.
			const auto cMaxStackSize = process.StackSize;

			process.StackReserve = (UInt8*)mm_new_ke_heap(cMaxStackSize, Yes, Yes);

			// if stack pointer isn't valid.
			if (!process.StackReserve)
			{
				process.StackReserve = (UInt8*)mm_new_ke_heap(kSchedMaxStackSz, Yes, Yes);
				kcout << "newoskrnl.exe: Use fallback reserve size.\r";
			}
		}
		else
		{
			if (process.Kind != UserProcess::kDLLKind)
			{
				process.Crash();
				return -kErrorProcessFault;
			}
		}

		process.Status = ProcessStatusKind::kStarting;

		process.ProcessId = mTeam.mProcessAmount;

		++mTeam.mProcessAmount;

		process.HeapCursor = process.HeapPtr;

		mTeam.AsArray()[process.ProcessId] = process;

		kcout << "UserProcessScheduler: Adding process to team [ OK ]...\r";

		return process.ProcessId;
	}

	/***********************************************************************************/

	UserProcessScheduler& UserProcessScheduler::The()
	{
		MUST_PASS(cProcessScheduler);
		return *cProcessScheduler;
	}

	/***********************************************************************************/

	/// @brief Remove process from list.
	/// @param processSlot process slot inside team.
	/// @retval true process was removed.
	/// @retval false process doesn't exist in team.
	Bool UserProcessScheduler::Remove(ProcessID processSlot)
	{
		// check if process is within range.
		if (processSlot > mTeam.AsArray().Count())
			return false;

		// also check if the process isn't a dummy one.
		if (mTeam.AsArray()[processSlot].Image == nullptr)
			return false;

		kcout << "UserProcessScheduler: Removing process...\r";

		mTeam.AsArray()[processSlot].Status = ProcessStatusKind::kDead;
		--mTeam.mProcessAmount;

		return true;
	}

	/// @brief Run scheduler.
	/// @return
	SizeT UserProcessScheduler::Run() noexcept
	{
		SizeT process_index = 0; //! we store this guy to tell the scheduler how many
								 //! things we have scheduled.

		for (; process_index < mTeam.AsArray().Capacity(); ++process_index)
		{
			auto& process = mTeam.AsArray()[process_index];

			//! check if process needs to be scheduled.
			if (UserProcessHelper::CanBeScheduled(process))
			{
				// set the current process.
				mTeam.AsRef() = process;

				process.PTime = static_cast<Int32>(process.Affinity);

				kcout << process.Name << ": will be runned.\r";

				// tell helper to find a core to schedule on.
				if (!UserProcessHelper::Switch(process.Image, &process.StackReserve[process.StackSize], process.StackFrame,
										   process.ProcessId))
				{
					process.Crash();
					continue;
				}

				process.Exit();

				continue;
			}
			else
			{
				// otherwise increment the P-time.
				--process.PTime;
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

	/// @brief Gets current running process.
	/// @return
	Ref<UserProcess>& UserProcessScheduler::CurrentProcess()
	{
		return mTeam.AsRef();
	}

	/// @brief Current proccess id getter.
	/// @return UserProcess ID integer.
	PID& UserProcessHelper::TheCurrentPID()
	{
		kcout << "UserProcessHelper::TheCurrentPID: Leaking ProcessId...\r";
		return cProcessScheduler->CurrentProcess().Leak().ProcessId;
	}

	/// @brief Check if process can be schedulded.
	/// @param process the process reference.
	/// @retval true can be schedulded.
	/// @retval false cannot be schedulded.
	bool UserProcessHelper::CanBeScheduled(UserProcess& process)
	{
		if (process.Status == ProcessStatusKind::kFrozen ||
			process.Status == ProcessStatusKind::kDead)
			return false;

		if (process.Kind == UserProcess::kDLLKind)
		{
			if (auto start = process.DLLPtr->Load<VoidPtr>(kPefStart, rt_string_len(kPefStart), kPefCode);
				start)
			{
				process.Image		   = start;
			}
		}

		return process.PTime < 1;
	}

	/**
	 * @brief Scheduler helper class.
	 */

	SizeT UserProcessHelper::StartScheduling()
	{
		if (!cProcessScheduler)
		{
			cProcessScheduler = new UserProcessScheduler();
			MUST_PASS(cProcessScheduler);

			kcout << "newoskrnl.exe: Team capacity: " << number(cProcessScheduler->CurrentTeam().AsArray().Capacity()) << endl;
		}

		SizeT ret = cProcessScheduler->Run();
		return ret;
	}

	/**
	 * \brief Does a context switch in a CPU.
	 * \param the_stack the stackframe of the running app.
	 * \param new_pid the process's PID.
	 */

	bool UserProcessHelper::Switch(VoidPtr image_ptr, UInt8* stack, HAL::StackFramePtr frame_ptr, const PID& new_pid)
	{
		if (!stack || !frame_ptr || !image_ptr || new_pid < 0)
			return false;

		kcout << "newoskrnl.exe: Finding hardware thread...\r";

		for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Count(); ++index)
		{
			if (HardwareThreadScheduler::The()[index].Leak()->Kind() == kInvalidHart)
				continue;

			if (HardwareThreadScheduler::The()[index].Leak()->IsBusy())
				continue;

			if (HardwareThreadScheduler::The()[index].Leak()->Kind() !=
					ThreadKind::kHartBoot &&
				HardwareThreadScheduler::The()[index].Leak()->Kind() !=
					ThreadKind::kHartSystemReserved)
			{
				HardwareThreadScheduler::The()[index].Leak()->Busy(true);

				UserProcessHelper::TheCurrentPID() = new_pid;

				kcout << "newoskrnl.exe: Found hardware thread...\r";

				bool ret = HardwareThreadScheduler::The()[index].Leak()->Switch(image_ptr, stack, frame_ptr);

				HardwareThreadScheduler::The()[index].Leak()->Busy(false);

				return ret;
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
