/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file ProcessScheduler.cxx
/// @brief ProcessHeader Scheduler API.
/***********************************************************************************/

#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/SMPManager.hpp>
#include <KernelKit/KernelHeap.hpp>
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

void ProcessHeader::Crash() {
  kcout << "ProcessScheduler: Crashed, ExitCode: -1.\r\n";
  MUST_PASS(!ke_bug_check());

  this->Exit(-1);
}

void ProcessHeader::Wake(const bool should_wakeup) {
  this->Status =
      should_wakeup ? ProcessStatus::kRunning : ProcessStatus::kFrozen;
}

/***********************************************************************************/

VoidPtr ProcessHeader::New(const SizeT &sz) {
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

  for (SizeT index = sz; _pool[sz] != kUserHeapMag; --index) {
    if (&_pool[index] > &_pool_ptr[sz]) continue;

    if (_pool[index] == _pool_ptr[index]) return true;
  }

  return false;
}

/* @brief free pointer from usage. */
Boolean ProcessHeader::Delete(VoidPtr ptr, const SizeT &sz) {
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

const Char *ProcessHeader::GetName() { return this->Name; }

const ProcessSelector &ProcessHeader::GetSelector() { return this->Selector; }

const ProcessStatus &ProcessHeader::GetStatus() { return this->Status; }

/***********************************************************************************/

/**
@brief Affinity is the time slot allowed for the process.
*/
const AffinityKind &ProcessHeader::GetAffinity() { return this->Affinity; }

/**
@brief Standard exit proc.
*/
void ProcessHeader::Exit(Int32 exit_code) {
  if (this->ProcessId !=
      ProcessScheduler::Shared().Leak().GetCurrent().Leak().ProcessId)
    ke_stop(RUNTIME_CHECK_PROCESS);

  if (this->Ring == (Int32)ProcessSelector::kRingKernel &&
      ProcessScheduler::Shared().Leak().GetCurrent().Leak().Ring > 0)
    ke_stop(RUNTIME_CHECK_PROCESS);

  kExitCode = exit_code;

  if (this->Ring != (Int32)ProcessSelector::kRingDriver) {
    if (this->HeapPtr) rt_free_heap(this->HeapPtr);

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

  ProcessScheduler::Shared().Leak().Remove(this->ProcessId);
}

SizeT ProcessScheduler::Add(Ref<ProcessHeader> &process) {
  if (!process) return -1;

  if (process.Leak().Ring != (Int32)ProcessSelector::kRingKernel) return -1;

  kcout << "ProcessScheduler::Add(Ref<ProcessHeader>& process)\r\n";

  process.Leak().HeapPtr = rt_new_heap(kUserHeapUser | kUserHeapRw);
  process.Leak().ProcessId = mTeam.AsArray().Count();
  process.Leak().HeapCursor = process.Leak().HeapPtr;

  process.Leak().StackFrame = reinterpret_cast<HAL::StackFrame *>(
      ke_new_ke_heap(sizeof(HAL::StackFrame), true, false));

  MUST_PASS(process.Leak().StackFrame);

  UIntPtr imageStart = reinterpret_cast<UIntPtr>(process.Leak().Image);

  process.Leak().SetStart(imageStart);

  mTeam.AsArray().Add(process);

  if (!imageStart && process.Leak().Kind == ProcessHeader::ExecutableType) {
    process.Leak().Crash();
  }

  if (!imageStart && process.Leak().Kind == ProcessHeader::DriverType) {
    if (process.Leak().Ring == 3)
      process.Leak().Crash();
    else
      ke_stop(RUNTIME_CHECK_PROCESS);
  }

  return mTeam.AsArray().Count() - 1;
}

bool ProcessScheduler::Remove(SizeT process) {
  if (process > mTeam.AsArray().Count()) return false;

  kcout << "ProcessScheduler::Remove(SizeT process)\r\n";

  return mTeam.AsArray().Remove(process);
}

SizeT ProcessScheduler::Run() noexcept {
  SizeT processIndex = 0;  //! we store this guy to tell the scheduler how many
                           //! things we have scheduled.

  for (; processIndex < mTeam.AsArray().Count(); ++processIndex) {
    auto process = mTeam.AsArray()[processIndex];

    MUST_PASS(process);  //! no need for a MUST_PASS(process.Leak());, it is
                         //! recursive because of the nature of the class;

    //! run any process needed to be scheduled.
    if (ProcessHelper::CanBeScheduled(process.Leak())) {
      auto unwrapped_process = *process.Leak();

      unwrapped_process.PTime = 0;

      // set the current process.
      mTeam.AsRef() = unwrapped_process;

      // tell helper to find a core to schedule on.
      ProcessHelper::Switch(mTeam.AsRef().Leak().StackFrame,
                            mTeam.AsRef().Leak().ProcessId);
    } else {
      // otherwise increment the P-time.
      ++mTeam.AsRef().Leak().PTime;
    }
  }

  return processIndex;
}

Ref<ProcessScheduler> ProcessScheduler::Shared() {
  static ProcessScheduler ref;
  return {ref};
}

Ref<ProcessHeader> &ProcessScheduler::GetCurrent() { return mTeam.AsRef(); }

PID &ProcessHelper::GetCurrentPID() {
  kcout << "ProcessHelper::GetCurrentPID: Leaking ProcessId...\r\n";
  return ProcessScheduler::Shared().Leak().GetCurrent().Leak().ProcessId;
}

bool ProcessHelper::CanBeScheduled(Ref<ProcessHeader> &process) {
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
 * @brief Spin scheduler class.
 */
bool ProcessHelper::StartScheduling() {
  if (ProcessHelper::CanBeScheduled(
          ProcessScheduler::Shared().Leak().GetCurrent())) {
    --ProcessScheduler::Shared().Leak().GetCurrent().Leak().PTime;
    return false;
  }

  auto processRef = ProcessScheduler::Shared().Leak();

  if (!processRef)
    return false;  // we have nothing to schedule. simply return.

  SizeT ret = processRef.Run();

  kcout << StringBuilder::FromInt(
      "ProcessHelper::StartScheduling() Iterated over {%} jobs inside team.\r\n", ret);

  return true;
}

/**
 * \brief Does a context switch in a CPU.
 * \param the_stack the stackframe of the running app.
 * \param new_pid the process's PID.
*/

bool ProcessHelper::Switch(HAL::StackFrame *the_stack, const PID &new_pid) {
  if (!the_stack || new_pid < 0) return false;

  for (SizeT index = 0UL; index < SMPManager::Shared().Leak().Count(); ++index) {
    if (SMPManager::Shared().Leak()[index].Leak().Kind() == kInvalidHart)
      continue;

    if (SMPManager::Shared().Leak()[index].Leak().StackFrame() == the_stack) {
      SMPManager::Shared().Leak()[index].Leak().Busy(false);
      continue;
    }

    if (SMPManager::Shared().Leak()[index].Leak().IsBusy()) continue;

    if (SMPManager::Shared().Leak()[index].Leak().Kind() !=
            ThreadKind::kHartBoot &&
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
