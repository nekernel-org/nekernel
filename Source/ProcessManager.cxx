/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/SMPManager.hpp>
#include <NewKit/KHeap.hpp>
#include <NewKit/String.hpp>

#define kPoolAlign (4)

///! bugs = 0

/***********************************************************************************/
/* This file handles
 * The preemptive multitasking of the OS.
 * For MT see SMPManager. */
/***********************************************************************************/

namespace hCore
{
    static Int32 kExitCode = 0;

    const Int32& rt_get_exit_code() noexcept { return kExitCode; }

    void Process::Crash()
    {
        kcout << this->Name << ": Crashed.";
        this->Exit(-1);
    }

    void Process::Wake(const bool should_wakeup)
    {
        this->Status = should_wakeup ? ProcessStatus::kRunning : ProcessStatus::kFrozen;
    }

    VoidPtr Process::New(const SizeT& sz)
    {
        // RAM allocation
        if (this->PoolCursor)
        {
            VoidPtr ptr = this->PoolCursor;
            this->PoolCursor = (VoidPtr)((UIntPtr)this->PoolCursor + (sizeof(sz) + kPoolAlign));

            return ptr;
        }

        //! TODO: Disk to RAM allocation

        return nullptr;
    }

    /* @brief checks if runtime pointer is in region. */
    bool rt_in_pool_region(VoidPtr pool_ptr, VoidPtr pool, const SizeT& sz)
    {
        Char* _pool_ptr = (Char*)pool_ptr;
        Char* _pool = (Char*)pool;

        for (SizeT index = sz; _pool[sz] != 0x55; --index)
        {
            if (&_pool[index] > &_pool_ptr[sz])
                continue;

            if (_pool[index] != _pool_ptr[index])
                return false;
        }

        return true;
    }

    /* @brief free pointer from usage. */
    Boolean Process::Delete(VoidPtr ptr, const SizeT& sz)
    {
        if (sz < 1 ||
        this->PoolCursor == this->Pool)
            return false;

        if (rt_in_pool_region(ptr, this->PoolCursor, this->UsedMemory))
        {
            this->PoolCursor = (VoidPtr)((UIntPtr)this->PoolCursor - (sizeof(sz) - kPoolAlign));
            rt_zero_memory(ptr, sz);

            return true;
        }

        return false;
    }

    const Char* Process::GetName() { return this->Name; }

    const ProcessSelector& Process::GetSelector() { return this->Selector; }

    const ProcessStatus& Process::GetStatus() { return this->Status; }

    const AffinityKind& Process::GetAffinity() { return this->Affinity; }

    void Process::Exit(Int32 exit_code)
    {
        if (this->ProcessId != ProcessManager::Shared().Leak().GetCurrent().Leak().ProcessId)
            return;

        if (this->Ring == (Int32)ProcessSelector::kRingKernel &&
            ProcessManager::Shared().Leak().GetCurrent().Leak().Ring > 0)
            return;

        kExitCode = exit_code;

        if (this->Ring != (Int32)ProcessSelector::kRingDriver &&
           this->Ring != (Int32)ProcessSelector::kRingKernel)
        {
            pool_free_ptr(this->Pool);

            this->PoolCursor = nullptr;

            this->FreeMemory = kPoolMaxSz;
            this->UsedMemory = 0UL;
        }

        //! Delete image if not done already.
        if (this->Image)
            kernel_delete_ptr(this->Image);

        if (this->StackFrame)
            kernel_delete_ptr((VoidPtr)this->StackFrame);

        ProcessManager::Shared().Leak().Remove(this->ProcessId);
    }

    bool ProcessManager::Add(Ref<Process>& process)
    {
        if (!process)
            return false;

        kcout << "ProcessManager::Add(Ref<Process>& process)\r\n";

        process.Leak().Pool = pool_new_ptr(kPoolUser | kPoolRw);
        process.Leak().ProcessId = this->m_Headers.Count();
        process.Leak().PoolCursor = process.Leak().Pool;

        process.Leak().StackFrame = reinterpret_cast<HAL::StackFrame*>(
            kernel_new_ptr(sizeof(HAL::StackFrame), true, false)
            );

        MUST_PASS(process.Leak().StackFrame);

        UIntPtr imageStart = reinterpret_cast<UIntPtr>(
                                 process.Leak().Image
            );

#ifdef __x86_64__
        process.Leak().StackFrame->Rbp = imageStart;
#elif defined(__powerpc)
        // link return register towards the __start symbol.
        process.Leak().StackFrame->R3 = imageStart;
#endif

        this->m_Headers.Add(process);

        return true;
    }

    bool ProcessManager::Remove(SizeT process)
    {
        if (process > this->m_Headers.Count())
            return false;

        kcout << "ProcessManager::Remove(SizeT process)\r\n";

        return this->m_Headers.Remove(process);
    }

    SizeT ProcessManager::Run() noexcept
    {
        SizeT processIndex = 0; //! we store this guy to tell the scheduler how many things we
        						//! have scheduled.

        for (; processIndex < this->m_Headers.Count(); ++processIndex)
        {
            auto process = this->m_Headers[processIndex];
            MUST_PASS(process); //! no need for a MUST_PASS(process.Leak());, it is recursive because of the nature of the class;

            //! run any process needed to be scheduled.
            if (ProcessHelper::CanBeScheduled(process.Leak()))
            {
                auto unwrapped_process = *process.Leak();

                unwrapped_process.PTime = 0;

                // set the current process.
                m_CurrentProcess = unwrapped_process;

                ProcessHelper::Switch(m_CurrentProcess.Leak().StackFrame,
                                      m_CurrentProcess.Leak().ProcessId);
            }
            else
            {
                // otherwise increment the micro-time.
                ++m_CurrentProcess.Leak().PTime;
            }
        }

        return processIndex;
    }

    Ref<ProcessManager> ProcessManager::Shared()
    {
        static ProcessManager ref;
        return { ref };
    }

    Ref<Process>& ProcessManager::GetCurrent()
    {
        return m_CurrentProcess;
    }

    PID& ProcessHelper::GetCurrentPID()
    {
        kcout << "ProcessHelper::GetCurrentPID\r\n";
        return ProcessManager::Shared().Leak().GetCurrent().Leak().ProcessId;
    }

    bool ProcessHelper::CanBeScheduled(Ref<Process>& process)
    {
        if (process.Leak().Status == ProcessStatus::kFrozen ||
            process.Leak().Status == ProcessStatus::kDead)
            return false;

        if (process.Leak().GetStatus() == ProcessStatus::kStarting)
        {
            if (process.Leak().PTime < static_cast<Int>(kMinMicroTime))
            {
                process.Leak().Status = ProcessStatus::kRunning;
                process.Leak().Affinity = AffinityKind::kStandard;

                return true;
            }

            ++process.Leak().PTime;
        }

        return process.Leak().PTime > static_cast<Int>(kMinMicroTime);
    }

    bool ProcessHelper::StartScheduling()
    {
        if (ProcessHelper::CanBeScheduled(ProcessManager::Shared().Leak().GetCurrent()))
        {
            --ProcessManager::Shared().Leak().GetCurrent().Leak().PTime;
            return false;
        }

        auto process_ref = ProcessManager::Shared().Leak();

        if (!process_ref)
            return false; // we have nothing to schedule. simply return.

        SizeT ret = process_ref.Run();

        kcout << StringBuilder::FromInt("ProcessHelper::Run() iterated over: % processes\r\n", ret);

        return true;
    }

    bool ProcessHelper::Switch(HAL::StackFrame* the_stack, const PID& new_pid)
    {
        if (!the_stack ||
            new_pid < 0)
            return false;

        for (SizeT index = 0UL; index < kMaxHarts; ++index)
        {
            if (SMPManager::Shared().Leak()[index].Leak().StackFrame() == the_stack)
            {
                SMPManager::Shared().Leak()[index].Leak().Busy(false);
                continue;
            }

            if (SMPManager::Shared().Leak()[index].Leak().IsBusy())
                continue;

            if (SMPManager::Shared().Leak()[index].Leak().Kind() != ThreadKind::kBoot ||
               SMPManager::Shared().Leak()[index].Leak().Kind() != ThreadKind::kSystemReserved)
            {
                SMPManager::Shared().Leak()[index].Leak().Busy(true);
                ProcessHelper::GetCurrentPID() = new_pid;

                return SMPManager::Shared().Leak()[index].Leak().Switch(the_stack);
            }
        }

        return false;
    }
} // namespace hCore
