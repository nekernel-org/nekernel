/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/***********************************************************************************/
/// @file ProcessManager.cxx
/// @brief Process Scheduler API.
/***********************************************************************************/

#include <KernelKit/HError.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/SMPManager.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/String.hpp>

///! bugs = 0

/***********************************************************************************/
/* This file handles the process scheduling.
/***********************************************************************************/

namespace HCore {
/***********************************************************************************/
/// Exit Code stuff
/***********************************************************************************/

static Int32 kExitCode = 0;

const Int32 &rt_get_exit_code() noexcept { return kExitCode; }

/***********************************************************************************/

void Process::Crash() {
  kcout << this->Name << ": Crashed, ExitCode: -1\n";
  MUST_PASS(!ke_bug_check());

  this->Exit(-1);
}

void Process::Wake(const bool should_wakeup) {
  this->Status =
      should_wakeup ? ProcessStatus::kRunning : ProcessStatus::kFrozen;
}

/***********************************************************************************/

VoidPtr Process::New(const SizeT &sz) {
  if (this->FreeMemory < 1) return nullptr;

  if (this->HeapCursor) {
    VoidPtr ptr = this->HeapCursor;
    this->HeapCursor = (VoidPtr)((UIntPtr)this->HeapCursor + (sizeof(sz)));

    ++this->UsedMemory;
    --this->FreeMemory;

    return ptr;
  }

  return nullptr;
}

/***********************************************************************************/

/* @brief checks if runtime pointer is in region. */
bool rt_in_pool_region(VoidPtr pool_ptr, VoidPtr pool, const SizeT &sz) {
  Char *_pool_ptr = (Char *)pool_ptr;
  Char *_pool = (Char *)pool;

  for (SizeT index = sz; _pool[sz] != 0x55; --index) {
    if (&_pool[index] > &_pool_ptr[sz]) continue;

    if (_pool[index] == _pool_ptr[index]) return true;
  }

  return false;
}

/* @brief free pointer from usage. */
Boolean Process::Delete(VoidPtr ptr, const SizeT &sz) {
  if (sz < 1 || this->HeapCursor == this->HeapPtr) return false;

  // also check for the amount of allocations we've done so far.
  if (this->UsedMemory < 1) return false;

  if (rt_in_pool_region(ptr, this->HeapCursor, this->UsedMemory)) {
    this->HeapCursor = (VoidPtr)((UIntPtr)this->HeapCursor - (sizeof(sz)));
    rt_zero_memory(ptr, sz);

    ++this->FreeMemory;
    --this->UsedMemory;

    return true;
  }

  return false;
}

const Char *Process::GetName() { return this->Name; }

const ProcessSelector &Process::GetSelector() { return this->Selector; }

const ProcessStatus &Process::GetStatus() { return this->Status; }

/***********************************************************************************/

/**
@brief Affinity is the time slot allowed for the process.
*/
const AffinityKind &Process::GetAffinity() { return this->Affinity; }

/**
@brief Standard exit proc.
*/
void Process::Exit(Int32 exit_code) {
  if (this->ProcessId !=
      ProcessManager::Shared().Leak().GetCurrent().Leak().ProcessId)
    ke_stop(RUNTIME_CHECK_PROCESS);

  if (this->Ring == (Int32)ProcessSelector::kRingKernel &&
      ProcessManager::Shared().Leak().GetCurrent().Leak().Ring > 0)
    ke_stop(RUNTIME_CHECK_PROCESS);

  kExitCode = exit_code;

  if (this->Ring != (Int32)ProcessSelector::kRingDriver) {
    if (this->HeapPtr) ke_free_heap(this->HeapPtr);

    this->HeapPtr = nullptr;
    this->HeapCursor = nullptr;

    this->FreeMemory = 0UL;  // TODO: fill available heap.
    this->UsedMemory = 0UL;
  }

  //! Delete image if not done already.
  if (this->Image) ke_delete_ke_heap(this->Image);
  if (this->StackFrame) ke_delete_ke_heap((VoidPtr)this->StackFrame);

  this->Image = nullptr;
  this->StackFrame = nullptr;

  ProcessManager::Shared().Leak().Remove(this->ProcessId);
}

bool ProcessManager::Add(Ref<Process> &process) {
  if (!process) return false;

  if (process.Leak().Ring != (Int32)ProcessSelector::kRingKernel) return false;

  kcout << "ProcessManager::Add(Ref<Process>& process)\r\n";

  process.Leak().HeapPtr = ke_new_heap(kPoolUser | kPoolRw);
  process.Leak().ProcessId = this->m_Headers.Count();
  process.Leak().HeapCursor = process.Leak().HeapPtr;

  process.Leak().StackFrame = reinterpret_cast<HAL::StackFrame *>(
      ke_new_ke_heap(sizeof(HAL::StackFrame), true, false));

  MUST_PASS(process.Leak().StackFrame);

  UIntPtr imageStart = reinterpret_cast<UIntPtr>(process.Leak().Image);

  process.Leak().AssignStart(imageStart);

  this->m_Headers.Add(process);

  if (!imageStart && process.Leak().Kind == Process::ExecutableType) {
    process.Leak().Crash();
  }

  if (!imageStart && process.Leak().Kind == Process::DriverType) {
    if (process.Leak().Ring == 3)
      process.Leak().Crash();
    else
      ke_stop(RUNTIME_CHECK_PROCESS);
  }

  return true;
}

bool ProcessManager::Remove(SizeT process) {
  if (process > this->m_Headers.Count()) return false;

  kcout << "ProcessManager::Remove(SizeT process)\r\n";

  return this->m_Headers.Remove(process);
}

SizeT ProcessManager::Run() noexcept {
  SizeT processIndex = 0;  //! we store this guy to tell the scheduler how many
                           //! things we have scheduled.

  for (; processIndex < this->m_Headers.Count(); ++processIndex) {
    auto process = this->m_Headers[processIndex];

    MUST_PASS(process);  //! no need for a MUST_PASS(process.Leak());, it is
                         //! recursive because of the nature of the class;

    //! run any process needed to be scheduled.
    if (ProcessHelper::CanBeScheduled(process.Leak())) {
      auto unwrapped_process = *process.Leak();

      unwrapped_process.PTime = 0;

      // set the current process.
      m_CurrentProcess = unwrapped_process;

      ProcessHelper::Switch(m_CurrentProcess.Leak().StackFrame,
                            m_CurrentProcess.Leak().ProcessId);
    } else {
      // otherwise increment the P-time.
      ++m_CurrentProcess.Leak().PTime;
    }
  }

  return processIndex;
}

Ref<ProcessManager> ProcessManager::Shared() {
  static ProcessManager ref;
  return {ref};
}

Ref<Process> &ProcessManager::GetCurrent() { return m_CurrentProcess; }

PID &ProcessHelper::GetCurrentPID() {
  kcout << "ProcessHelper::GetCurrentPID\r\n";
  return ProcessManager::Shared().Leak().GetCurrent().Leak().ProcessId;
}

bool ProcessHelper::CanBeScheduled(Ref<Process> &process) {
  if (process.Leak().Status == ProcessStatus::kFrozen ||
      process.Leak().Status == ProcessStatus::kDead)
    return false;

  if (process.Leak().GetStatus() == ProcessStatus::kStarting) {
    if (process.Leak().PTime < static_cast<Int>(kMinMicroTime)) {
      process.Leak().Status = ProcessStatus::kRunning;
      process.Leak().Affinity = AffinityKind::kHartStandard;

      return true;
    }

    ++process.Leak().PTime;
  }

  return process.Leak().PTime > static_cast<Int>(kMinMicroTime);
}

/**
 * @brief Scheduler spin code.
 */
bool ProcessHelper::StartScheduling() {
  if (ProcessHelper::CanBeScheduled(
          ProcessManager::Shared().Leak().GetCurrent())) {
    --ProcessManager::Shared().Leak().GetCurrent().Leak().PTime;
    return false;
  }

  auto process_ref = ProcessManager::Shared().Leak();

  if (!process_ref)
    return false;  // we have nothing to schedule. simply return.

  SizeT ret = process_ref.Run();

  kcout << StringBuilder::FromInt(
      "ProcessHelper::StartScheduling() Iterated over: % processes.\r\n", ret);

  return true;
}

bool ProcessHelper::Switch(HAL::StackFrame *the_stack, const PID &new_pid) {
  if (!the_stack || new_pid < 0) return false;

  for (SizeT index = 0UL; index < kMaxHarts; ++index) {
    if (SMPManager::Shared().Leak()[index].Leak().Kind() == kInvalidHart)
      continue;

    if (SMPManager::Shared().Leak()[index].Leak().StackFrame() == the_stack) {
      SMPManager::Shared().Leak()[index].Leak().Busy(false);
      continue;
    }

    if (SMPManager::Shared().Leak()[index].Leak().IsBusy()) continue;

    if (SMPManager::Shared().Leak()[index].Leak().Kind() !=
            ThreadKind::kHartBoot ||
        SMPManager::Shared().Leak()[index].Leak().Kind() !=
            ThreadKind::kHartSystemReserved) {
      SMPManager::Shared().Leak()[index].Leak().Busy(true);
      ProcessHelper::GetCurrentPID() = new_pid;

      return SMPManager::Shared().Leak()[index].Leak().Switch(the_stack);
    }
  }

  return false;
}
}  // namespace HCore
