/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: UserProcessScheduler.cxx
	PURPOSE: User sided process scheduler.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessScheduler.cxx
/// @brief User Process scheduler.
/***********************************************************************************/

#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/IPEFDLLObject.hxx>
#include <KernelKit/HardwareThreadScheduler.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/String.hxx>
#include <KernelKit/LPC.hxx>

///! BUGS: 0

/***********************************************************************************/
/** TODO: Document the Kernel, SDK and kits. */
/***********************************************************************************/

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Exit Code global variable.
	/***********************************************************************************/

	STATIC UInt32 cLastExitCode = 0U;

	/***********************************************************************************/
	/// @brief User Process scheduler global object.
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
		if (*this->Name == 0)
			return;

		kcout << this->Name << ": crashed, ID = " << number(kErrorProcessFault) << endl;

		this->Exit(kErrorProcessFault);
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

	/** @brief Add pointer to entry. */
	VoidPtr UserProcess::New(const SizeT& sz)
	{
#ifdef __ZKA_AMD64__
		auto pd = hal_read_cr3();
		hal_write_cr3(this->MemoryPD);

		auto ptr = mm_new_ke_heap(sz, Yes, Yes);

		hal_write_cr3(reinterpret_cast<UIntPtr>(pd));
#else
		auto ptr = mm_new_ke_heap(sz, Yes, Yes);
#endif

		if (!this->MemoryEntryList)
		{
			this->MemoryEntryList			   = new UserProcess::PROCESS_MEMORY_ENTRY();
			this->MemoryEntryList->MemoryEntry = ptr;

			this->MemoryEntryList->MemoryPrev = nullptr;
			this->MemoryEntryList->MemoryNext = nullptr;

			return ptr;
		}
		else
		{
			auto entry = this->MemoryEntryList;

			while (entry->MemoryNext)
			{
				if (entry->MemoryNext)
					entry = entry->MemoryNext;
			}

			entry->MemoryNext			   = new UserProcess::PROCESS_MEMORY_ENTRY();
			entry->MemoryNext->MemoryEntry = ptr;

			entry->MemoryNext->MemoryPrev = entry;
			entry->MemoryNext->MemoryNext = nullptr;
		}

		return nullptr;
	}

	/***********************************************************************************/

	/** @brief Free pointer from usage. */
	Boolean UserProcess::Delete(VoidPtr ptr, const SizeT& sz)
	{
		auto entry = this->MemoryEntryList;

		while (entry->MemoryNext)
		{
			if (entry->MemoryEntry == ptr)
			{
#ifdef __ZKA_AMD64__
				auto pd = hal_read_cr3();
				hal_write_cr3(this->MemoryPD);

				bool ret = mm_delete_ke_heap(ptr);
				hal_write_cr3(reinterpret_cast<UIntPtr>(pd));

				return ret;
#else
				bool ret = mm_delete_ke_heap(ptr);
				return ret;
#endif
			}

			if (entry->MemoryNext)
				entry = entry->MemoryNext;
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
		if (this->Image && mm_is_valid_heap(this->Image))
			mm_delete_ke_heap(this->Image);

		if (this->StackFrame && mm_is_valid_heap(this->StackFrame))
			mm_delete_ke_heap((VoidPtr)this->StackFrame);

		this->Image		 = nullptr;
		this->StackFrame = nullptr;

		if (this->Kind == kDLLKind)
		{
			Bool success = false;
			rtl_fini_dll(this, this->DLLPtr, &success);

			if (success)
			{
				this->DLLPtr = nullptr;
			}
		}

		if (this->StackReserve)
			delete[] this->StackReserve;

		this->ProcessId = 0;

		if (this->ProcessId > 0)
			UserProcessScheduler::The().Remove(this->ProcessId);
	}

	/// @brief Add process to list.
	/// @param process the process *Ref* class.
	/// @return the process index inside the team.
	SizeT UserProcessScheduler::Add(UserProcess& process)
	{
#ifdef __ZKA_AMD64__
		process.MemoryPD = reinterpret_cast<UIntPtr>(hal_read_cr3());
#endif // __ZKA_AMD64__

		process.StackFrame = new HAL::StackFrame(0);

		if (!process.StackFrame)
		{
			process.Crash();
			return -kErrorProcessFault;
		}

		// Create heap according to type of process.
		if (process.Kind == UserProcess::kDLLKind)
		{
			process.DLLPtr = rtl_init_dll(&process);
		}

		if (!process.Image)
		{
			if (process.Kind != UserProcess::kDLLKind)
			{
				process.Crash();
				return -kErrorProcessFault;
			}
		}

		// get preferred stack size by app.
		const auto cMaxStackSize = process.StackSize;
		process.StackReserve	 = new UInt8[cMaxStackSize];

		if (!process.StackReserve)
		{
			process.Crash();
			return -kErrorProcessFault;
		}

		if (mTeam.mProcessAmount > kSchedProcessLimitPerTeam)
			mTeam.mProcessAmount = 0UL;

		++mTeam.mProcessAmount;

		process.ProcessId = mTeam.mProcessAmount;
		process.Status	  = ProcessStatusKind::kStarting;

		mTeam.AsArray()[process.ProcessId] = process;

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
	/// @param process_id process slot inside team.
	/// @retval true process was removed.
	/// @retval false process doesn't exist in team.
	Bool UserProcessScheduler::Remove(ProcessID process_id)
	{
		// check if process is within range.
		if (process_id > mTeam.AsArray().Count())
			return false;

		mTeam.AsArray()[process_id].Status = ProcessStatusKind::kDead;
		--mTeam.mProcessAmount;

		return true;
	}

	/// @brief Run scheduler.
	/// @return
	SizeT UserProcessScheduler::Run() noexcept
	{
		SizeT process_index = 0; //! we store this guy to tell the scheduler how many
								 //! things we have scheduled.

		kcout << "Finding available process...\r";

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
	bool UserProcessHelper::CanBeScheduled(const UserProcess process)
	{
		if (process.Status == ProcessStatusKind::kFrozen ||
			process.Status == ProcessStatusKind::kDead)
			return false;

		return process.PTime < 1;
	}

	/**
	 * @brief Scheduler helper class.
	 */

	EXTERN
	HardwareThreadScheduler* cHardwareThreadScheduler;

	SizeT UserProcessHelper::StartScheduling()
	{
		if (!cHardwareThreadScheduler)
		{
			cHardwareThreadScheduler = new HardwareThreadScheduler();
		}

		if (!cProcessScheduler)
		{
			cProcessScheduler = new UserProcessScheduler();
		}

		SizeT ret = cProcessScheduler->Run();
		return ret;
	}

	/**
	 * \brief Does a context switch in a CPU.
	 * \param the_stack the stackframe of the running app.
	 * \param new_pid the process's PID.
	 */

	Bool UserProcessHelper::Switch(VoidPtr image_ptr, UInt8* stack, HAL::StackFramePtr frame_ptr, const PID& new_pid)
	{
		if (!stack || !frame_ptr || !image_ptr || new_pid < 0)
			return No;

		for (SizeT index = 0UL; index < HardwareThreadScheduler::The().Count(); ++index)
		{
			if (HardwareThreadScheduler::The()[index].Leak()->Kind() == kInvalidHart)
				continue;

			if (HardwareThreadScheduler::The()[index].Leak()->Kind() !=
					ThreadKind::kHartBoot &&
				HardwareThreadScheduler::The()[index].Leak()->Kind() !=
					ThreadKind::kHartSystemReserved)
			{
				PID prev_pid					   = UserProcessHelper::TheCurrentPID();
				UserProcessHelper::TheCurrentPID() = new_pid;

				bool ret = HardwareThreadScheduler::The()[index].Leak()->Switch(image_ptr, stack, frame_ptr);

				if (!ret)
				{
					UserProcessHelper::TheCurrentPID() = prev_pid;
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
