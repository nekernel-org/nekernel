/* -------------------------------------------

	Copyright ZKA Web Services Co.

	FILE: UserProcessScheduler.cc
	PURPOSE: EL0/Ring-3 Process scheduler.

------------------------------------------- */

/***********************************************************************************/
/// @file UserProcessScheduler.cc
/// @brief User process scheduler.
/***********************************************************************************/

#include <ArchKit/ArchKit.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/IPEFDLLObject.h>
#include <KernelKit/HardwareThreadScheduler.h>
#include <KernelKit/Heap.h>
#include <NewKit/String.h>
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

	STATIC UInt32 cLastExitCode = 0U;

	/***********************************************************************************/
	/// @brief User Process scheduler global and external reference of thread scheduler.
	/***********************************************************************************/

	UserProcessScheduler*			kProcessScheduler = nullptr;
	EXTERN HardwareThreadScheduler* kHardwareThreadScheduler;

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
		if (this->Status != ProcessStatusKind::kRunning)
			return;

		if (*this->Name != 0)
		{
			kcout << this->Name << ": crashed, error id: " << number(kErrorProcessFault) << endl;
		}

		this->Exit(kErrorProcessFault);
	}

	/***********************************************************************************/
	//! @brief boolean operator, check status.
	/***********************************************************************************/

	UserProcess::operator bool()
	{
		return this->Status != ProcessStatusKind::kDead;
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
		return fLocalCode;
	}

	/***********************************************************************************/
	/// @brief Wake process.
	/***********************************************************************************/

	void UserProcess::Wake(const bool should_wakeup)
	{
		this->Status =
			should_wakeup ? ProcessStatusKind::kRunning : ProcessStatusKind::kFrozen;
	}

	/***********************************************************************************/
	/** @brief Add pointer to entry. */
	/***********************************************************************************/

	VoidPtr UserProcess::New(const SizeT& sz)
	{
#ifdef __ZKA_AMD64__
		auto pd = hal_read_cr3();
		hal_write_cr3(reinterpret_cast<VoidPtr>(this->VMRegister));

		auto ptr = mm_new_heap(sz, Yes, Yes);

		hal_write_cr3(reinterpret_cast<VoidPtr>(pd));
#else
		auto ptr = mm_new_heap(sz, Yes, Yes);
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
			PROCESS_MEMORY_ENTRY* entry		 = this->MemoryEntryList;
			PROCESS_MEMORY_ENTRY* prev_entry = nullptr;

			while (!entry)
			{
				if (entry->MemoryEntry == nullptr)
					break; // chose to break here, when we get an already allocated memory entry for our needs.

				prev_entry = entry;
				entry	   = entry->MemoryNext;
			}

			entry->MemoryNext			   = new PROCESS_MEMORY_ENTRY();
			entry->MemoryNext->MemoryEntry = ptr;

			entry->MemoryNext->MemoryPrev = entry;
			entry->MemoryNext->MemoryNext = nullptr;
		}

		return nullptr;
	}

	/***********************************************************************************/
	/** @brief Free pointer from usage. */
	/***********************************************************************************/

	Boolean UserProcess::Delete(VoidPtr ptr, const SizeT& sz)
	{
		if (!ptr ||
			sz == 0)
			return No;

		PROCESS_MEMORY_ENTRY* entry = this->MemoryEntryList;

		while (entry != nullptr)
		{
			if (entry->MemoryEntry == ptr)
			{
#ifdef __ZKA_AMD64__
				auto pd = hal_read_cr3();
				hal_write_cr3(reinterpret_cast<VoidPtr>(this->VMRegister));

				auto ret = mm_delete_heap(entry->MemoryEntry);

				hal_write_cr3(pd);

				return ret;
#else
				Bool ret = mm_delete_heap(ptr);
				return ret;
#endif
			}

			entry = entry->MemoryNext;
		}

		return No;
	}

	/***********************************************************************************/
	/// @brief Gets the name of the current process.
	/***********************************************************************************/

	const Char* UserProcess::GetProcessName() noexcept
	{
		return this->Name;
	}

	/***********************************************************************************/
	/// @brief Gets the owner of the process.
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
	@brief Affinity is the time slot allowed for the process.
	*/
	/***********************************************************************************/

	const AffinityKind& UserProcess::GetAffinity() noexcept
	{
		return this->Affinity;
	}

	/***********************************************************************************/
	/**
	@brief Process exit method.
	*/
	/***********************************************************************************/

	void UserProcess::Exit(const Int32& exit_code)
	{
		this->Status = ProcessStatusKind::kDead;

		fLastExitCode = exit_code;
		cLastExitCode = exit_code;

		auto memory_list = this->MemoryEntryList;

		// Deleting memory lists. Make sure to free all of them.
		while (memory_list)
		{
			if (memory_list->MemoryEntry)
			{
#ifdef __ZKA_AMD64__
				auto pd = hal_read_cr3();
				hal_write_cr3(reinterpret_cast<VoidPtr>(this->VMRegister));
#endif

				MUST_PASS(mm_delete_heap(memory_list->MemoryEntry));

#ifdef __ZKA_AMD64__
				hal_write_cr3(pd);
#endif
			}

			auto next = memory_list->MemoryNext;

			mm_delete_heap(memory_list);
			memory_list = nullptr;

			memory_list = next;
		}

		//! Free the memory's page directory.
		HAL::mm_free_bitmap(reinterpret_cast<VoidPtr>(this->VMRegister));

		//! Delete image if not done already.
		if (this->Image && mm_is_valid_heap(this->Image))
			mm_delete_heap(this->Image);

		if (this->StackFrame && mm_is_valid_heap(this->StackFrame))
			mm_delete_heap((VoidPtr)this->StackFrame);

		this->Image		 = nullptr;
		this->StackFrame = nullptr;

		if (this->Kind == kExectuableDLLKind)
		{
			Bool success = false;
			rtl_fini_dll(this, this->PefDLLDelegate, &success);

			if (success)
			{
				this->PefDLLDelegate = nullptr;
			}
		}

		if (this->StackReserve)
			delete[] this->StackReserve;

		this->ProcessId = 0;

		if (this->ProcessId > 0)
			UserProcessScheduler::The().Remove(this->ProcessId);
	}

	/***********************************************************************************/
	/// @brief Add process to list.
	/// @param process the process *Ref* class.
	/// @return the process index inside the team.
	/***********************************************************************************/

	SizeT UserProcessScheduler::Add(UserProcess process)
	{
		if (mTeam.mProcessAmount > kSchedProcessLimitPerTeam)
			return -kErrorInvalidData;

#ifdef __ZKA_AMD64__
		process.VMRegister = reinterpret_cast<UIntPtr>(HAL::mm_alloc_bitmap(Yes, Yes, sizeof(PDE), Yes));
#endif // __ZKA_AMD64__

		process.StackFrame = (HAL::StackFramePtr)mm_new_heap(sizeof(HAL::StackFrame), Yes, Yes);

		if (!process.StackFrame)
		{
			process.Crash();
			return -kErrorProcessFault;
		}

		// Create heap according to type of process.
		if (process.Kind == UserProcess::kExectuableDLLKind)
		{
			process.PefDLLDelegate = rtl_init_dll(&process);
		}

		if (!process.Image)
		{
			if (process.Kind != UserProcess::kExectuableDLLKind)
			{
				process.Crash();
				return -kErrorProcessFault;
			}
		}

		// get preferred stack size by app.
		const auto cMaxStackSize = process.StackSize;
		process.StackReserve	 = (UInt8*)mm_new_heap(sizeof(UInt8) * cMaxStackSize, Yes, Yes);

		if (!process.StackReserve)
		{
			process.Crash();
			return -kErrorProcessFault;
		}

		++mTeam.mProcessAmount;

		process.ProcessId = mTeam.mProcessAmount;
		process.Status	  = ProcessStatusKind::kStarting;

		mTeam.AsArray()[process.ProcessId] = process;

		return process.ProcessId;
	}

	/***********************************************************************************/
	/// @brief Retrieves the singleton of the process scheduler.
	/***********************************************************************************/

	UserProcessScheduler& UserProcessScheduler::The()
	{
		MUST_PASS(kProcessScheduler);
		return *kProcessScheduler;
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
		if (process_id > mTeam.AsArray().Count())
			return No;

		mTeam.AsArray()[process_id].Status = ProcessStatusKind::kDead;
		--mTeam.mProcessAmount;

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
	/// @return Process executed within team.

	/***********************************************************************************/

	SizeT UserProcessScheduler::Run() noexcept
	{
		SizeT process_index = 0; //! we store this guy to tell the scheduler how many
								 //! things we have scheduled.

		if (mTeam.mProcessAmount < 1)
		{
			kcout << "UserProcessScheduler::Run(): This team doesn't have any process!\r";
			return 0;
		}

		for (; process_index < mTeam.AsArray().Capacity(); ++process_index)
		{
			auto& process = mTeam.AsArray()[process_index];

			//! check if process needs to be scheduled.
			if (UserProcessHelper::CanBeScheduled(process))
			{
				process.PTime = static_cast<Int32>(process.Affinity);

				UserProcessScheduler::The().CurrentProcess().Leak().Status = ProcessStatusKind::kFrozen;
				UserProcessScheduler::The().CurrentProcess()			   = process;

				kcout << "Switch to '" << process.Name << "'.\r";

				// Set current process header.
				this->CurrentProcess() = process;

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
				if (process.Status == ProcessStatusKind::kRunning)
					--process.PTime;
			}
		}

		kcout << "Scheduled Process Count: " << number(process_index) << endl;

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
		return kProcessScheduler->CurrentProcess().Leak().ProcessId;
	}

	/// @brief Check if process can be schedulded.
	/// @param process the process reference.
	/// @retval true can be schedulded.
	/// @retval false cannot be schedulded.
	Bool UserProcessHelper::CanBeScheduled(const UserProcess& process)
	{
		if (process.Status == ProcessStatusKind::kKilled ||
			process.Status == ProcessStatusKind::kDead)
			return No;

		if (process.Status == ProcessStatusKind::kStarting &&
			process.Image)
			return Yes;

		if (!process.Image &&
			process.Kind == UserProcess::kExectuableKind)
			return No;

		return process.PTime < 1 && process.Status == ProcessStatusKind::kRunning;
	}

	/***********************************************************************************/
	/**
	 * @brief Allocate a scheduler.
	 */
	/***********************************************************************************/

	Bool UserProcessHelper::InitializeScheduler()
	{
		if (!kProcessScheduler)
		{
			kProcessScheduler = new UserProcessScheduler();
		}

		if (!kHardwareThreadScheduler)
		{
			kHardwareThreadScheduler = new HardwareThreadScheduler();
		}

		return Yes;
	}

	/***********************************************************************************/
	/**
	 * @brief Start scheduling current AP/Hart/Core.
	 */
	/***********************************************************************************/
	SizeT UserProcessHelper::StartScheduling()
	{
		if (!kProcessScheduler)
			return 0;

		return kProcessScheduler->Run();
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

				auto prev_ptime										 = HardwareThreadScheduler::The()[index].Leak()->fPTime;
				HardwareThreadScheduler::The()[index].Leak()->fPTime = UserProcessScheduler::The().CurrentTeam().AsArray()[new_pid].ProcessId;
				Bool ret											 = HardwareThreadScheduler::The()[index].Leak()->Switch(image_ptr, stack, frame_ptr);

				if (!ret)
				{
					HardwareThreadScheduler::The()[index].Leak()->fPTime = prev_ptime;
					UserProcessHelper::TheCurrentPID()					 = prev_pid;

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
